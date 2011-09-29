#include "HistoryWindow.h"
#include "ui_HistoryWindow.h"

#include "ProtocolManager.h"
#include "AccountManager.h"
#include "SettingsWindow.h"
#include "ContactManager.h"
#include "SDK/constants.h"
#include "SDK/Contact.h"
#include "IconManager.h"
#include "Profile.h"
#include "Core.h"

#include <QtGui/QStandardItemModel>
#include <QtGui/QTreeWidgetItem>
#include <QtGui/QStandardItem>
#include <QtGui/QKeyEvent>
#include <QtCore/QFileInfo>
#include <QtCore/QDebug>
#include <QtCore/QDir>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlError>
#include <QtSql/QSqlQuery>
#include <QtWebKit/QWebFrame>

#define qDebug() qDebug() << "[HistoryWindow]"
#define qWarning() qWarning() << "[HistoryWindow]"

using namespace Kitty;
using namespace KittySDK;

Kitty::ContactProxy::ContactProxy(QObject *parent): QSortFilterProxyModel(parent)
{
  setDynamicSortFilter(true);
  setFilterCaseSensitivity(Qt::CaseInsensitive);
  setSourceModel(new QStandardItemModel());
}

bool Kitty::ContactProxy::filterAcceptsRow(int row, const QModelIndex &parent) const
{
  QModelIndex index = sourceModel()->index(row, 0, parent);

  //root is always visible
  if(!parent.isValid()) {
    return true;
  }

  //accounts are only visible if atleast one contact is visible
  if(sourceModel()->hasChildren(index)) {
    for(int i = 0; i < sourceModel()->rowCount(index); i++) {
      if(QSortFilterProxyModel::filterAcceptsRow(i, index)) {
        return true;
      }
    }

    return false;
  }

  //contact are accepted the usual way
  return QSortFilterProxyModel::filterAcceptsRow(row, parent);
}


Kitty::HistoryWindow::HistoryWindow(QWidget *parent): QWidget(parent), m_ui(new Ui::HistoryWindow)
{
  Core *core = Core::inst();

  m_ui->setupUi(this);

  qDebug() << "Creating";

  m_proxy = new ContactProxy(this);
  m_ui->contactTree->setModel(m_proxy);

  connect(IconManager::inst(), SIGNAL(iconsUpdated()), this, SLOT(updateIcons()));
  connect(core->settingsWindow(), SIGNAL(settingsApplied()), this, SLOT(applySettings()));
  connect(m_ui->contactTree->selectionModel(), SIGNAL(selectionChanged(QItemSelection,QItemSelection)), this, SLOT(loadChats(QItemSelection,QItemSelection)));

  restoreGeometry(core->setting(Settings::S_HISTORYWINDOW_GEOMETRY).toByteArray());

  m_ui->chatWebView->setAutoScroll(false);

  m_ui->dateToEdit->setDate(QDate::currentDate());
  m_ui->chatTree->header()->restoreState(core->setting(Settings::S_HISTORYWINDOW_COLUMNS, m_ui->chatTree->header()->saveState()).toByteArray());
  m_ui->filtersButton->setChecked(core->setting(Settings::S_HISTORYWINDOW_FILTERS, false).toBool());

  applySettings();
  updateIcons();
}

Kitty::HistoryWindow::~HistoryWindow()
{
  Core *core = Core::inst();

  core->setSetting(Settings::S_HISTORYWINDOW_GEOMETRY, saveGeometry());
  core->setSetting(Settings::S_HISTORYWINDOW_COLUMNS, m_ui->chatTree->header()->saveState());
  core->setSetting(Settings::S_HISTORYWINDOW_FILTERS, m_ui->filtersButton->isChecked());

  delete m_ui;
}

void Kitty::HistoryWindow::applySettings()
{

}

void Kitty::HistoryWindow::updateIcons()
{
  Core *core = Core::inst();

  m_ui->refreshButton->setIcon(core->icon(Icons::I_REFRESH));
  m_ui->exportButton->setIcon(core->icon(Icons::I_FILE));
  m_ui->importButton->setIcon(core->icon(Icons::I_FOLDER));
  m_ui->printButton->setIcon(core->icon(Icons::I_PRINTER));
  m_ui->filtersButton->setIcon(core->icon(Icons::I_FILTER));
  m_ui->searchButton->setIcon(core->icon(Icons::I_SEARCH));
}

void Kitty::HistoryWindow::showContact(KittySDK::Contact *contact)
{
  if(contact) {
    show();
    activateWindow();

    m_ui->contactSearchEdit->clear();

    QModelIndex index = findContact(contact);
    if(index.isValid()) {
      m_ui->contactTree->setCurrentIndex(index);
    }
  }
}

void Kitty::HistoryWindow::showEvent(QShowEvent *event)
{
  Core *core = Core::inst();

  m_ui->searchEdit->clear();
  m_ui->contactSearchEdit->clear();
  static_cast<QStandardItemModel*>(m_proxy->sourceModel())->clear();
  m_ui->chatTree->clear();
  m_ui->chatWebView->clear();

  if(QStandardItem *root = static_cast<QStandardItemModel*>(m_proxy->sourceModel())->invisibleRootItem()) {
    QStandardItem *conversations = new QStandardItem();
    conversations->setText(tr("Conversations"));
    conversations->setIcon(core->icon(Icons::I_FOLDER));
    conversations->setData(HistoryWindow::ItemFolder, HistoryWindow::RoleType);

    QDir historyDir(core->currentProfileDir() + "history/");
    if(historyDir.exists()) {
      foreach(const QString &protoDir, historyDir.entryList(QDir::Dirs | QDir::NoDotAndDotDot)) {
          Protocol *proto = ProtocolManager::inst()->protocolByName(protoDir);

          historyDir.cd(protoDir);
          foreach(const QString &accountDir, historyDir.entryList(QDir::Dirs | QDir::NoDotAndDotDot)) {
              Account *acc = 0;

              QStandardItem *accountItem = new QStandardItem();
              accountItem->setText(accountDir);
              accountItem->setData(HistoryWindow::ItemAccount, HistoryWindow::RoleType);
              accountItem->setData(accountDir, HistoryWindow::RolePathName);
              accountItem->setData(protoDir, HistoryWindow::RoleProtocol);

              if(proto) {
                accountItem->setIcon(core->icon(proto->protoInfo()->protoIcon()));

                acc = AccountManager::inst()->account(proto, accountDir);
              } else {
                accountItem->setIcon(core->icon(Icons::I_BULLET));
              }

              historyDir.cd(accountDir);
              foreach(const QFileInfo &contactFile, historyDir.entryInfoList(QStringList("*.db"), QDir::Files)) {
                Contact *contact = 0;

                QStandardItem *contactItem = new QStandardItem();
                contactItem->setData(HistoryWindow::ItemContact, HistoryWindow::RoleType);
                contactItem->setData(protoDir, HistoryWindow::RoleProtocol);
                contactItem->setData(accountDir, HistoryWindow::RoleAccount);
                contactItem->setData(contactFile.fileName(), HistoryWindow::RolePathName);

                if(acc) {
                  contact = acc->contacts().value(contactFile.completeBaseName());
                }

                if(contact) {
                  contactItem->setText(contact->display());
                } else {
                  contactItem->setText(contactFile.completeBaseName());
                }

                if(proto) {
                  contactItem->setIcon(core->icon(proto->protoInfo()->protoIcon()));
                } else {
                  contactItem->setIcon(core->icon(Icons::I_BULLET));
                }

                accountItem->appendRow(contactItem);
              }
              historyDir.cdUp();

              if(accountItem->rowCount() > 0) {
                conversations->appendRow(accountItem);
              } else {
                delete accountItem;
              }
          }
          historyDir.cdUp();
      }
    }

    root->appendRow(conversations);

    m_ui->contactTree->expandAll();
  }

  QWidget::showEvent(event);
}

void Kitty::HistoryWindow::on_contactSearchEdit_textChanged(const QString &text)
{
  m_proxy->setFilterWildcard(text);
  m_ui->contactTree->expandAll();
}

void Kitty::HistoryWindow::on_chatTree_currentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous)
{
  Core *core = Core::inst();

  m_ui->chatWebView->clear();

  if(current) {
    QModelIndex index = m_ui->contactTree->currentIndex();

    int type = index.data(HistoryWindow::RoleType).toInt();
    switch(type) {
      case HistoryWindow::ItemAccount:
      {

      }
      break;

      case HistoryWindow::ItemContact:
      {
        QString filePath = core->currentProfileDir() + QString("history/%1/%2/%3").arg(index.data(HistoryWindow::RoleProtocol).toString()).arg(index.data(HistoryWindow::RoleAccount).toString()).arg(index.data(HistoryWindow::RolePathName).toString());

        Account *acc = AccountManager::inst()->account(index.data(HistoryWindow::RoleProtocol).toString(), index.data(HistoryWindow::RoleAccount).toString());
        Contact *contact = 0;
        if(acc) {
          contact = acc->contacts().value(QFileInfo(index.data(HistoryWindow::RolePathName).toString()).completeBaseName());
        }

        if(QFile(filePath).exists()) {
          QSqlDatabase db = QSqlDatabase::database();
          if(!db.isValid()) {
            db = QSqlDatabase::addDatabase("QSQLITE");
          }

          db.setDatabaseName(filePath);
          if(!db.open()) {
            qDebug() << "Failed to open db" << db.databaseName() << db.lastError().text();
            return;
          }

          QSqlQuery query;

          QString where;
          switch(m_ui->directionComboBox->currentIndex()) {
            case 1:
              where = QString("AND dir = %1 ").arg(Message::Incoming);
            break;

            case 2:
              where = QString("AND dir = %1 ").arg(Message::Outgoing);
            break;

            default:
            break;
          }

          if(m_ui->dateFromEdit->date() != QDate(1970, 1, 1)) {
            where.append(QString("AND timeStamp >= %1 ").arg(QDateTime(m_ui->dateFromEdit->date()).toTime_t()));
          }

          if(m_ui->dateToEdit->date() != QDate::currentDate()) {
            where.append(QString("AND timeStamp <= %1 ").arg(QDateTime(m_ui->dateToEdit->date()).toTime_t()));
          }

          if(!m_ui->searchEdit->text().isEmpty()) {
            where.append("AND body LIKE :search");
          }

          query.prepare(QString("SELECT * FROM messages WHERE chatId=:chatId %1 ORDER BY timeStamp ASC;").arg(where));
          query.bindValue("chatId", current->text(3));

          if(!m_ui->searchEdit->text().isEmpty()) {
            query.bindValue("search", "%" + m_ui->searchEdit->text() + "%");
          }

          if(query.exec()) {
            while(query.next()) {
              Message::Direction dir = (Message::Direction)query.value(3).toInt();

              QString uid = QFileInfo(index.data(HistoryWindow::RolePathName).toString()).completeBaseName();

              Contact cnt(uid, 0);
              cnt.setDisplay(uid);

              Contact me(acc->uid(), acc);
              me.setDisplay(core->profile()->name());

              Message *msg = 0;

              if(dir == Message::Incoming || dir == Message::System) {
                if(contact) {
                  msg = new Message(contact, &me);
                } else {
                  msg = new Message(&cnt, &me);
                }
              } else if(dir == Message::Outgoing) {
                if(contact) {
                  msg = new Message(&me, contact);
                } else {
                  msg = new Message(&me, &cnt);
                }
              }

              if(msg) {
                msg->setBody(query.value(4).toString());
                msg->setDirection(dir);
                msg->setTimeStamp(QDateTime::fromTime_t(query.value(2).toInt()));

                m_ui->chatWebView->appendMessage(*msg);
              }

              delete msg;
            }

            m_ui->chatWebView->page()->mainFrame()->setScrollPosition(QPoint());
          } else {
            qDebug() << "Error executing query" << query.lastQuery() << query.lastError().text();
          }

          query.clear();
          db.close();
        }
      }
      break;
    }
  }
}

QModelIndex Kitty::HistoryWindow::findContact(KittySDK::Contact *contact, const QModelIndex &parent)
{
  for(int i = 0; i < m_proxy->rowCount(parent); i++) {
    QModelIndex index = m_proxy->index(i, 0, parent);

    if(index.data(HistoryWindow::RoleType).toInt() == HistoryWindow::ItemContact) {
      if(index.data(HistoryWindow::RoleProtocol).toString() == contact->protocol()->protoInfo()->protoName()) {
        if(index.data(HistoryWindow::RoleAccount).toString() == contact->account()->uid()) {
          if(QFileInfo(index.data(HistoryWindow::RolePathName).toString()).completeBaseName() == contact->uid()) {
            return index;
          }
        }
      }
    }

    if(m_proxy->rowCount(index) > 0) {
      QModelIndex result = findContact(contact, index);
      if(result.isValid()) {
        return result;
      }
    }
  }

  return QModelIndex();
}

void Kitty::HistoryWindow::loadChats(const QItemSelection &selected, const QItemSelection &deselected)
{
  if(selected.indexes().count() > 0) {
    QModelIndex current = m_ui->contactTree->currentIndex();

    Core *core = Core::inst();

    m_ui->chatTree->clear();

    int type = current.data(HistoryWindow::RoleType).toInt();
    switch(type) {
      case HistoryWindow::ItemFolder:
      {
        m_ui->chatTree->setColumnHidden(1, true);
        m_ui->chatTree->headerItem()->setText(0, tr("Account"));
      }
      break;

      case HistoryWindow::ItemAccount:
      {
        m_ui->chatTree->headerItem()->setText(0, tr("Contact"));
        m_ui->chatTree->setColumnHidden(1, true);

        QSqlDatabase db = QSqlDatabase::database();
        if(!db.isValid()) {
          db = QSqlDatabase::addDatabase("QSQLITE");
        }

        Account *acc = AccountManager::inst()->account(current.data(HistoryWindow::RoleProtocol).toString(), current.data().toString());

        QDir accountPath = core->currentProfileDir() + QString("history/%1/%2/").arg(current.data(HistoryWindow::RoleProtocol).toString()).arg(current.data(HistoryWindow::RolePathName).toString());
        if(accountPath.exists()) {
          foreach(const QFileInfo &contactFile, accountPath.entryInfoList(QStringList("*.db"), QDir::Files)) {
            QTreeWidgetItem *item = new QTreeWidgetItem(m_ui->chatTree);
            Contact *contact = 0;

            if(acc) {
              contact = acc->contacts().value(contactFile.completeBaseName());
              item->setIcon(0, core->icon(acc->protocol()->protoInfo()->protoIcon()));
            } else {
              item->setIcon(0, core->icon(Icons::I_BULLET));
            }

            if(contact) {
              item->setText(0, contact->display());
            } else {
              item->setText(0, contactFile.completeBaseName());
            }

            QString filePath = contactFile.absoluteFilePath();
            if(QFile(filePath).exists()) {
              db.setDatabaseName(filePath);
              if(!db.open()) {
                qDebug() << "Failed to open db" << db.databaseName() << db.lastError().text();
                return;
              }

              QSqlQuery query(" SELECT"
                              "   COUNT(DISTINCT chatId) as chats,"
                              "   msg.count as messages "
                              " FROM"
                              "   messages"
                              "   JOIN (SELECT COUNT(*) as count FROM messages) msg;");
              query.next();

              int chats = query.value(0).toInt();
              int messages = query.value(1).toInt();
              item->setText(2, QString("%1 %2, %3 %4").arg(chats).arg(tr("chat(s)", "", chats)).arg(messages).arg(tr("message(s)", "", messages)));

              query.clear();
              db.close();
            }
          }
        }
      }
      break;

      case HistoryWindow::ItemContact:
      {
        QString filePath = core->currentProfileDir() + QString("history/%1/%2/%3").arg(current.data(HistoryWindow::RoleProtocol).toString()).arg(current.data(HistoryWindow::RoleAccount).toString()).arg(current.data(HistoryWindow::RolePathName).toString());

        m_ui->chatTree->headerItem()->setText(0, tr("Message"));
        m_ui->chatTree->setColumnHidden(1, false);

        Account *acc = AccountManager::inst()->account(current.data(HistoryWindow::RoleProtocol).toString(), current.data(HistoryWindow::RoleAccount).toString());

        if(QFile(filePath).exists()) {
          QSqlDatabase db = QSqlDatabase::database();
          if(!db.isValid()) {
            db = QSqlDatabase::addDatabase("QSQLITE");
          }

          db.setDatabaseName(filePath);
          if(!db.open()) {
            qDebug() << "Failed to open db" << db.databaseName() << db.lastError().text();
            return;
          }

          QSqlQuery query(" SELECT"
                          "    *,"
                          "   COUNT(*) as 'count'"
                          " FROM ("
                          "   SELECT"
                          "     m.*,"
                          "     lt.lastTimeStamp"
                          "   FROM"
                          "     messages m"
                          "     JOIN (SELECT MAX(timeStamp) as lastTimeStamp, chatId FROM messages GROUP BY chatId) lt"
                          "     ON lt.chatId = m.chatId"
                          "     ORDER BY timeStamp DESC"
                          "   )"
                          " GROUP BY chatId"
                          " ORDER BY timeStamp DESC;");
          while(query.next()) {
              QTreeWidgetItem *item = new QTreeWidgetItem(m_ui->chatTree);

              QString text = query.value(4).toString().remove(QRegExp("<[^>]*>"));
              if(text.length() > 60) {
                 text = text.left(60).append("...");
              }

              if(acc) {
                item->setIcon(0, core->icon(acc->protocol()->protoInfo()->protoIcon()));
              } else {
                item->setIcon(0, core->icon(Icons::I_BULLET));
              }

              item->setText(0, text);
              item->setText(1, QString("%1 - %2").arg(QDateTime::fromTime_t(query.value(2).toInt()).toString(Qt::SystemLocaleShortDate)).arg(QDateTime::fromTime_t(query.value(5).toInt()).toString(Qt::SystemLocaleShortDate)));
              item->setText(2, query.value(6).toString());
              item->setText(3, query.value(1).toString());
          }

          query.clear();
          db.close();
        }
      }
      break;
    }

    m_ui->chatTree->setFocus();
    if(m_ui->chatTree->topLevelItemCount() > 0) {
      m_ui->chatTree->setCurrentItem(m_ui->chatTree->topLevelItem(0));
    }
  }
}

void Kitty::HistoryWindow::updateCurrentChat()
{
  on_chatTree_currentItemChanged(m_ui->chatTree->currentItem(), 0);
}

void Kitty::HistoryWindow::on_filtersButton_toggled(bool checked)
{
  if(!checked) {
    m_ui->directionComboBox->setCurrentIndex(0);
    m_ui->dateFromEdit->setDate(QDate(1970, 1, 1));
    m_ui->dateToEdit->setDate(QDate::currentDate());
  }
}

void Kitty::HistoryWindow::on_chatTree_doubleClicked(const QModelIndex &current)
{
  if(current.isValid()) {
    QModelIndex parent = m_ui->contactTree->currentIndex();
    if(parent.data(HistoryWindow::RoleType).toInt() == HistoryWindow::ItemAccount) {
      for(int i = 0; i < m_proxy->rowCount(parent); i++) {
        QModelIndex index = m_proxy->index(i, 0, parent);
        if(index.data() == current.data()) {
          m_ui->contactTree->setCurrentIndex(index);
        }
      }
    }
  }
}
