#include "HistoryWindow.h"
#include "ui_HistoryWindow.h"

#include "ProtocolManager.h"
#include "AccountManager.h"
#include "SettingsWindow.h"
#include "ContactManager.h"
#include "SDK/constants.h"
#include "SDK/Contact.h"
#include "IconManager.h"
#include "Core.h"

#include <QtGui/QStandardItemModel>
#include <QtGui/QStandardItem>
#include <QtGui/QKeyEvent>
#include <QtCore/QFileInfo>
#include <QtCore/QDebug>
#include <QtCore/QDir>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlError>
#include <QtSql/QSqlQuery>

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
  m_ui->contactTree->installEventFilter(this);

  qDebug() << "Creating";

  m_proxy = new ContactProxy(this);
  m_ui->contactTree->setModel(m_proxy);

  connect(IconManager::inst(), SIGNAL(iconsUpdated()), this, SLOT(updateIcons()));
  connect(core->settingsWindow(), SIGNAL(settingsApplied()), this, SLOT(applySettings()));

  restoreGeometry(core->setting(Settings::S_HISTORYWINDOW_GEOMETRY).toByteArray());

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
}

void Kitty::HistoryWindow::showEvent(QShowEvent *event)
{
  Core *core = Core::inst();

  m_ui->searchEdit->clear();
  m_ui->contactSearchEdit->clear();
  static_cast<QStandardItemModel*>(m_proxy->sourceModel())->clear();

  QStandardItem *root = static_cast<QStandardItemModel*>(m_proxy->sourceModel())->invisibleRootItem();

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

  QWidget::showEvent(event);
}

bool Kitty::HistoryWindow::eventFilter(QObject *obj, QEvent *event)
{
  if(obj == m_ui->contactTree) {
    if(event->type() == QEvent::KeyPress) {
      QKeyEvent *ev = static_cast<QKeyEvent*>(event);
      if(ev) {
        if((ev->key() == Qt::Key_Up) || (ev->key() == Qt::Key_Down)) {
          on_contactTree_clicked(m_ui->contactTree->currentIndex());
        }
      }
    }
  }

  return obj->eventFilter(obj, event);
}

void Kitty::HistoryWindow::on_contactSearchEdit_textChanged(const QString &text)
{
  m_proxy->setFilterWildcard(text);
}

void Kitty::HistoryWindow::on_contactTree_clicked(const QModelIndex &index)
{
  Core *core = Core::inst();

  m_ui->chatTree->clear();

  int type = index.data(Qt::UserRole + 1).toInt();
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

      Account *acc = AccountManager::inst()->account(index.data(HistoryWindow::RoleProtocol).toString(), index.data().toString());

      QDir accountPath = core->currentProfileDir() + QString("history/%1/%2/").arg(index.data(HistoryWindow::RoleProtocol).toString()).arg(index.data(HistoryWindow::RolePathName).toString());
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
        }
      }
    }
    break;

    case HistoryWindow::ItemContact:
    {
      QString filePath = core->currentProfileDir() + QString("history/%1/%2/%3").arg(index.data(HistoryWindow::RoleProtocol).toString()).arg(index.data(HistoryWindow::RoleAccount).toString()).arg(index.data(HistoryWindow::RolePathName).toString());

      m_ui->chatTree->headerItem()->setText(0, tr("Message"));
      m_ui->chatTree->setColumnHidden(1, false);

      Account *acc = AccountManager::inst()->account(index.data(HistoryWindow::RoleProtocol).toString(), index.data(HistoryWindow::RoleAccount).toString());

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

        QSqlQuery query("SELECT *, COUNT(*) as 'count' FROM (SELECT m.*, lt.lastTimeStamp FROM messages m JOIN (SELECT MAX(timeStamp) as lastTimeStamp, chatId FROM messages GROUP BY chatId) lt ON lt.chatId = m.chatId ORDER BY timeStamp DESC) GROUP BY chatId;");
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
            item->setText(1, QString("%1 - %2").arg(query.value(2).toString()).arg(query.value(5).toString()));
            item->setText(2, query.value(6).toString());
            item->setText(3, query.value(2).toString());
        }
      }
    }
    break;
  }
}

