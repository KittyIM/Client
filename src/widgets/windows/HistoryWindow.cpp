#include "HistoryWindow.h"
#include "ui_HistoryWindow.h"

#include "ProtocolManager.h"
#include "AccountManager.h"
#include "SettingsWindow.h"
#include "ContactManager.h"
#include "IconManager.h"
#include "Profile.h"
#include "Core.h"

#include <SDKConstants.h>
#include <IContact.h>

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

namespace Kitty
{

ContactProxy::ContactProxy(QObject *parent): QSortFilterProxyModel(parent)
{
	setDynamicSortFilter(true);
	setFilterCaseSensitivity(Qt::CaseInsensitive);
	setSourceModel(new QStandardItemModel());
}

bool ContactProxy::filterAcceptsRow(int row, const QModelIndex &parent) const
{
	QModelIndex index = sourceModel()->index(row, 0, parent);

	//root is always visible
	if(!parent.isValid()) {
		return true;
	}

	//accounts are only visible if atleast one contact is visible
	if(sourceModel()->hasChildren(index)) {
		for(int i = 0; i < sourceModel()->rowCount(index); ++i) {
			if(QSortFilterProxyModel::filterAcceptsRow(i, index)) {
				return true;
			}
		}

		return false;
	}

	//contact are accepted the usual way
	return QSortFilterProxyModel::filterAcceptsRow(row, parent);
}


HistoryWindow::HistoryWindow(QWidget *parent): QWidget(parent), m_ui(new Ui::HistoryWindow)
{
	Core *core = Core::inst();

	m_ui->setupUi(this);

	qDebug() << "Creating";

	m_proxy = new ContactProxy(this);
	m_ui->contactTree->setModel(m_proxy);

	connect(IconManager::inst(), SIGNAL(iconsUpdated()), SLOT(updateIcons()));
	connect(core->settingsWindow(), SIGNAL(settingsApplied()), SLOT(applySettings()));
	connect(m_ui->contactTree->selectionModel(), SIGNAL(selectionChanged(QItemSelection,QItemSelection)), SLOT(loadChats(QItemSelection,QItemSelection)));
	connect(m_ui->contactSearchEdit, SIGNAL(textChanged(QString)), SLOT(filterContacts(QString)));
	connect(m_ui->filtersButton, SIGNAL(toggled(bool)), SLOT(toggleFilters(bool)));
	connect(m_ui->refreshButton, SIGNAL(clicked()), SLOT(refreshChats()));

	restoreGeometry(core->setting(KittySDK::Settings::S_HISTORYWINDOW_GEOMETRY).toByteArray());
	m_ui->splitter->restoreState(core->setting(KittySDK::Settings::S_HISTORYWINDOW_SPLITTER).toByteArray());

	m_ui->chatWebView->setAutoScroll(false);

	m_ui->dateToEdit->setDate(QDate::currentDate());
	m_ui->chatTree->header()->restoreState(core->setting(KittySDK::Settings::S_HISTORYWINDOW_COLUMNS, m_ui->chatTree->header()->saveState()).toByteArray());
	m_ui->filtersButton->setChecked(core->setting(KittySDK::Settings::S_HISTORYWINDOW_FILTERS, false).toBool());

	applySettings();
	updateIcons();
}

HistoryWindow::~HistoryWindow()
{
	Core *core = Core::inst();

	core->setSetting(KittySDK::Settings::S_HISTORYWINDOW_GEOMETRY, saveGeometry());
	core->setSetting(KittySDK::Settings::S_HISTORYWINDOW_SPLITTER, m_ui->splitter->saveState());
	core->setSetting(KittySDK::Settings::S_HISTORYWINDOW_COLUMNS, m_ui->chatTree->header()->saveState());
	core->setSetting(KittySDK::Settings::S_HISTORYWINDOW_FILTERS, m_ui->filtersButton->isChecked());

	delete m_ui;
}

void HistoryWindow::applySettings()
{

}

void HistoryWindow::updateIcons()
{
	Core *core = Core::inst();

	m_ui->refreshButton->setIcon(core->icon(KittySDK::Icons::I_REFRESH));
	m_ui->exportButton->setIcon(core->icon(KittySDK::Icons::I_FILE));
	m_ui->importButton->setIcon(core->icon(KittySDK::Icons::I_FOLDER));
	m_ui->printButton->setIcon(core->icon(KittySDK::Icons::I_PRINTER));
	m_ui->filtersButton->setIcon(core->icon(KittySDK::Icons::I_FILTER));
	m_ui->searchButton->setIcon(core->icon(KittySDK::Icons::I_SEARCH));
}

void HistoryWindow::showContact(KittySDK::IContact *contact)
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

void HistoryWindow::showEvent(QShowEvent *event)
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
		conversations->setIcon(core->icon(KittySDK::Icons::I_FOLDER));
		conversations->setData(HistoryWindow::ItemFolder, HistoryWindow::TypeRole);

		QDir historyDir(core->currentProfileDir() + "history/");
		if(historyDir.exists()) {
			foreach(const QString &protoDir, historyDir.entryList(QDir::Dirs | QDir::NoDotAndDotDot)) {
				KittySDK::IProtocol *proto = ProtocolManager::inst()->protocolByName(protoDir);

				historyDir.cd(protoDir);
				foreach(const QString &accountDir, historyDir.entryList(QDir::Dirs | QDir::NoDotAndDotDot)) {
					KittySDK::IAccount *acc = 0;

					QStandardItem *accountItem = new QStandardItem();
					accountItem->setText(accountDir);
					accountItem->setData(HistoryWindow::ItemAccount, HistoryWindow::TypeRole);
					accountItem->setData(accountDir, HistoryWindow::PathNameRole);
					accountItem->setData(protoDir, HistoryWindow::ProtocolRole);

					if(proto) {
						accountItem->setIcon(core->icon(proto->protoInfo()->protoIcon()));

						acc = AccountManager::inst()->account(proto, accountDir);
					} else {
						accountItem->setIcon(core->icon(KittySDK::Icons::I_BULLET));
					}

					historyDir.cd(accountDir);
					foreach(const QFileInfo &contactFile, historyDir.entryInfoList(QStringList("*.db"), QDir::Files)) {
						KittySDK::IContact *contact = 0;

						QStandardItem *contactItem = new QStandardItem();
						contactItem->setData(HistoryWindow::ItemContact, HistoryWindow::TypeRole);
						contactItem->setData(protoDir, HistoryWindow::ProtocolRole);
						contactItem->setData(accountDir, HistoryWindow::AccountRole);
						contactItem->setData(contactFile.fileName(), HistoryWindow::PathNameRole);

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
							contactItem->setIcon(core->icon(KittySDK::Icons::I_BULLET));
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

void HistoryWindow::changeEvent(QEvent *event)
{
	if(event->type() == QEvent::LanguageChange) {
		m_ui->retranslateUi(this);
	}

	QWidget::changeEvent(event);
}

void HistoryWindow::filterContacts(const QString &filter)
{
	m_proxy->setFilterWildcard(filter);
	m_ui->contactTree->expandAll();
}

void HistoryWindow::on_chatTree_currentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous)
{
	Core *core = Core::inst();

	m_ui->chatWebView->clear();

	if(current) {
		QModelIndex index = m_ui->contactTree->currentIndex();

		int type = index.data(HistoryWindow::TypeRole).toInt();
		switch(type) {
			case HistoryWindow::ItemAccount:
			{

			}
			break;

			case HistoryWindow::ItemContact:
			{
				QString filePath = core->currentProfileDir() + QString("history/%1/%2/%3").arg(index.data(HistoryWindow::ProtocolRole).toString()).arg(index.data(HistoryWindow::AccountRole).toString()).arg(index.data(HistoryWindow::PathNameRole).toString());

				KittySDK::IAccount *acc = AccountManager::inst()->account(index.data(HistoryWindow::ProtocolRole).toString(), index.data(HistoryWindow::AccountRole).toString());
				KittySDK::IContact *contact = 0;
				if(acc) {
					contact = acc->contacts().value(QFileInfo(index.data(HistoryWindow::PathNameRole).toString()).completeBaseName());
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
							where = QString("AND dir = %1 ").arg(KittySDK::IMessage::Incoming);
						break;

						case 2:
							where = QString("AND dir = %1 ").arg(KittySDK::IMessage::Outgoing);
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
							KittySDK::IMessage::Direction dir = (KittySDK::IMessage::Direction)query.value(3).toInt();

							QString uid = QFileInfo(index.data(HistoryWindow::PathNameRole).toString()).completeBaseName();

							KittySDK::IContact cnt(uid, 0);
							cnt.setDisplay(uid);

							KittySDK::IContact me(acc->uid(), acc);
							me.setDisplay(core->profileName());

							KittySDK::IMessage *msg = 0;

							if(dir == KittySDK::IMessage::Incoming || dir == KittySDK::IMessage::System) {
								if(contact) {
									msg = new KittySDK::IMessage(contact, &me);
								} else {
									msg = new KittySDK::IMessage(&cnt, &me);
								}
							} else if(dir == KittySDK::IMessage::Outgoing) {
								if(contact) {
									msg = new KittySDK::IMessage(&me, contact);
								} else {
									msg = new KittySDK::IMessage(&me, &cnt);
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

QModelIndex HistoryWindow::findContact(KittySDK::IContact *contact, const QModelIndex &parent)
{
	for(int i = 0; i < m_proxy->rowCount(parent); ++i) {
		QModelIndex index = m_proxy->index(i, 0, parent);

		if(index.data(HistoryWindow::TypeRole).toInt() == HistoryWindow::ItemContact) {
			if(index.data(HistoryWindow::ProtocolRole).toString() == contact->protocol()->protoInfo()->protoName()) {
				if(index.data(HistoryWindow::AccountRole).toString() == contact->account()->uid()) {
					if(QFileInfo(index.data(HistoryWindow::PathNameRole).toString()).completeBaseName() == contact->uid()) {
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

void HistoryWindow::loadChats(const QItemSelection &selected, const QItemSelection &deselected)
{
	if(selected.indexes().count() > 0) {
		QModelIndex current = m_ui->contactTree->currentIndex();

		Core *core = Core::inst();

		m_ui->chatTree->clear();

		int type = current.data(HistoryWindow::TypeRole).toInt();
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

				KittySDK::IAccount *acc = AccountManager::inst()->account(current.data(HistoryWindow::ProtocolRole).toString(), current.data().toString());

				QDir accountPath = core->currentProfileDir() + QString("history/%1/%2/").arg(current.data(HistoryWindow::ProtocolRole).toString()).arg(current.data(HistoryWindow::PathNameRole).toString());
				if(accountPath.exists()) {
					foreach(const QFileInfo &contactFile, accountPath.entryInfoList(QStringList("*.db"), QDir::Files)) {
						QTreeWidgetItem *item = new QTreeWidgetItem();
						KittySDK::IContact *contact = 0;

						if(acc) {
							contact = acc->contacts().value(contactFile.completeBaseName());
							item->setIcon(0, core->icon(acc->protocol()->protoInfo()->protoIcon()));
						} else {
							item->setIcon(0, core->icon(KittySDK::Icons::I_BULLET));
						}

						if(contact) {
							item->setText(0, contact->display());
						} else {
							item->setText(0, contactFile.completeBaseName());
						}

						if(!m_ui->contactSearchEdit->text().isEmpty()) {
							if(!item->text(0).contains(m_ui->contactSearchEdit->text(), Qt::CaseInsensitive)) {
								delete item;
								continue;
							}
						}

						QString filePath = contactFile.absoluteFilePath();
						if(QFile(filePath).exists()) {
							db.setDatabaseName(filePath);
							if(!db.open()) {
								qDebug() << "Failed to open db" << db.databaseName() << db.lastError().text();
								delete item;
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
							item->setText(2, tr("%n chat(s)", "", chats) + ", " + tr("%n message(s)", "", messages));

							query.clear();
							db.close();

							m_ui->chatTree->addTopLevelItem(item);
						}
					}
				}
			}
			break;

			case HistoryWindow::ItemContact:
			{
				QString filePath = core->currentProfileDir() + QString("history/%1/%2/%3").arg(current.data(HistoryWindow::ProtocolRole).toString()).arg(current.data(HistoryWindow::AccountRole).toString()).arg(current.data(HistoryWindow::PathNameRole).toString());

				m_ui->chatTree->headerItem()->setText(0, tr("Message"));
				m_ui->chatTree->setColumnHidden(1, false);

				KittySDK::IAccount *acc = AccountManager::inst()->account(current.data(HistoryWindow::ProtocolRole).toString(), current.data(HistoryWindow::AccountRole).toString());

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
									"   *,"
									"   COUNT(*) as 'count'"
									" FROM ("
									"   SELECT"
									"     m.*,"
									"     lt.lastTimeStamp"
									"   FROM"
									"     messages m"
									"     JOIN (SELECT MAX(timeStamp) as lastTimeStamp, chatId FROM messages GROUP BY chatId) lt"
									"     ON"
									"       lt.chatId = m.chatId"
									"     ORDER BY"
									"       timeStamp DESC,"
									"       dir DESC"
									"   )"
									" GROUP BY chatId"
									" ORDER BY timeStamp DESC;");
					while(query.next()) {
						QTreeWidgetItem *item = new QTreeWidgetItem(m_ui->chatTree);

						QString text = query.value(4).toString().remove(QRegExp("<[^>]*>")).remove("\n");
						if(text.length() > 60) {
							text = text.left(60).append("...");
						}

						if(acc && acc->protocol()) {
							item->setIcon(0, core->icon(acc->protocol()->protoInfo()->protoIcon()));
						} else {
							item->setIcon(0, core->icon(KittySDK::Icons::I_BULLET));
						}

						QDateTime startTime = QDateTime::fromTime_t(query.value(2).toInt());
						QDateTime endTime = QDateTime::fromTime_t(query.value(5).toInt());

						QString duration = startTime.toString("h:mm");
						if(startTime.date() != endTime.date()) {
							duration += startTime.toString(" d.MM.yyyy");
						}

						duration += " - " + endTime.toString("h:mm d.MM.yyyy");

						item->setText(0, text);
						item->setText(1, duration);
						item->setText(2, query.value(6).toString());
						item->setText(3, query.value(1).toString());
					}

					query.clear();
					db.close();
				}
			}
			break;
		}

		if(!m_ui->contactSearchEdit->hasFocus()) {
			m_ui->chatTree->setFocus();
		}

		if(m_ui->chatTree->topLevelItemCount() > 0) {
			m_ui->chatTree->setCurrentItem(m_ui->chatTree->topLevelItem(0));
		}
	}
}

void HistoryWindow::updateCurrentChat()
{
	on_chatTree_currentItemChanged(m_ui->chatTree->currentItem(), 0);
}

void HistoryWindow::toggleFilters(bool checked)
{
	if(!checked) {
		m_ui->directionComboBox->setCurrentIndex(0);
		m_ui->dateFromEdit->setDate(QDate(1970, 1, 1));
		m_ui->dateToEdit->setDate(QDate::currentDate());
	}
}

void HistoryWindow::on_chatTree_doubleClicked(const QModelIndex &current)
{
	if(current.isValid()) {
		QModelIndex parent = m_ui->contactTree->currentIndex();
		if(parent.data(HistoryWindow::TypeRole).toInt() == HistoryWindow::ItemAccount) {
			for(int i = 0; i < m_proxy->rowCount(parent); ++i) {
				QModelIndex index = m_proxy->index(i, 0, parent);
				if(index.data() == current.data()) {
					m_ui->contactTree->setCurrentIndex(index);
				}
			}
		}
	}
}

}

void Kitty::HistoryWindow::refreshChats()
{
	loadChats(m_ui->contactTree->selectionModel()->selection(), QItemSelection());
}
