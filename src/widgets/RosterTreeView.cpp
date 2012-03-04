#include "RosterTreeView.h"

#include "widgets/windows/SettingsWindow.h"
#include "widgets/windows/ContactWindow.h"
#include "widgets/windows/MainWindow.h"
#include "RosterItemDelegate.h"
#include "RosterSortProxy.h"
#include "RosterItemModel.h"
#include "ContactManager.h"
#include "RosterContact.h"
#include "MessageQueue.h"
#include "ChatManager.h"
#include "RosterItem.h"
#include "Core.h"

#include <SDKConstants.h>
#include <IContact.h>
#include <IMessage.h>

#include <QtCore/QDebug>
#include <QtCore/QTimer>
#include <QtCore/QUrl>
#include <QtGui/QDesktopServices>
#include <QtGui/QInputDialog>
#include <QtGui/QApplication>
#include <QtGui/QMouseEvent>
#include <QtGui/QClipboard>
#include <QtGui/QMenu>

#define qDebug() qDebug() << "[RosterTreeView]"
#define qWarning() qWarning() << "[RosterTreeView]"

namespace Kitty
{

RosterTreeView::RosterTreeView(QWidget *parent): QTreeView(parent)
{
	setItemDelegate(new RosterItemDelegate(this));
	//setDragEnabled(true);
	//viewport()->setAcceptDrops(true);
	//setDropIndicatorShown(true);
	//setDragDropMode(QAbstractItemView::InternalMove);

	connect(this, SIGNAL(expanded(QModelIndex)), SLOT(itemExpanded(QModelIndex)));
	connect(this, SIGNAL(collapsed(QModelIndex)), SLOT(itemCollapsed(QModelIndex)));
	connect(MessageQueue::inst(), SIGNAL(messageEnqueued(quint32,KittySDK::IMessage)), SLOT(setupBlinking(quint32,KittySDK::IMessage)));
	connect(MessageQueue::inst(), SIGNAL(messageDequeued(quint32)), SLOT(unblinkIcon(quint32)));
}

void RosterTreeView::fixGroups()
{
	QModelIndex root = rootIndex();
	for(int i = 0; i < model()->rowCount(root); ++i) {
		QModelIndex index = model()->index(i, 0, root);
		if(index.data(RosterItem::TypeRole).toInt() == RosterItem::Group) {
			setExpanded(index, index.data(RosterItem::ExpandedRole).toBool());
		}
	}
}

void RosterTreeView::selectFirst()
{
	for(int i = 0; i < model()->rowCount(); ++i) {
		QModelIndex idx = model()->index(i, 0, rootIndex());
		QModelIndex index = dynamic_cast<RosterSortProxy*>(model())->mapToSource(idx);

		if(index.data(RosterItem::TypeRole).toInt() == RosterItem::Contact) {
			selectionModel()->setCurrentIndex(index, QItemSelectionModel::ClearAndSelect);
			return;
		} else if(index.data(RosterItem::TypeRole).toInt() == RosterItem::Group) {
			if(model()->rowCount(idx)) {
				selectionModel()->setCurrentIndex(model()->index(0, 0, idx), QItemSelectionModel::ClearAndSelect);
				return;
			}
		}
	}

}

void RosterTreeView::sendMessage()
{
	QModelIndexList list = selectedIndexes();

	if(list.count()) {
		if(RosterSortProxy *proxy = dynamic_cast<RosterSortProxy*>(model())) {
			QList<KittySDK::IContact*> contacts;
			KittySDK::IContact *me = 0;

			foreach(QModelIndex in, list) {
				QModelIndex index = proxy->mapToSource(in);
				if(index.data(RosterItem::TypeRole) == RosterItem::Contact) {
					if(RosterContact *cnt = static_cast<RosterContact*>(index.internalPointer())) {
						//for 1st one we set the account used
						if(in == list.first()) {
							me = cnt->contact()->account()->me();
							contacts << cnt->contact();

						//only add for the same account
						} else if(me == cnt->contact()->account()->me()) {
							contacts << cnt->contact();
						}
					}
				}
			}

			if(contacts.count()) {
				Core::inst()->mainWindow()->hideFilterEdit();
				ChatManager::inst()->startChat(me, contacts);
			}
		}
	}
}

void RosterTreeView::copyName()
{
	QModelIndexList list = selectedIndexes();

	if(RosterSortProxy *proxy = dynamic_cast<RosterSortProxy*>(model())) {
		QString names;

		foreach(QModelIndex index, list) {
			QModelIndex ind = proxy->mapToSource(index);
			names += ind.data().toString() + ", ";
		}

		names.chop(2);

		if(list.size() > 0) {
			qApp->clipboard()->setText(names);
		}
	}
}

void RosterTreeView::copyUid()
{
	QModelIndexList list = selectedIndexes();

	if(RosterSortProxy* proxy = dynamic_cast<RosterSortProxy*>(model())) {
		QString uids;

		foreach(QModelIndex index, list) {
			QModelIndex ind = proxy->mapToSource(index);
			uids += ind.data(RosterItem::UidRole).toString() + ", ";
		}

		uids.chop(2);

		if(list.size() > 0) {
			qApp->clipboard()->setText(uids);
		}
	}
}

void RosterTreeView::copyDescription()
{
	QModelIndexList list = selectedIndexes();
	if(list.size() > 0) {
		QModelIndex index = dynamic_cast<RosterSortProxy*>(model())->mapToSource(list.first());
		qApp->clipboard()->setText(index.data(RosterItem::DescriptionRole).toString());
	}
}

void RosterTreeView::moveToGroup()
{

	if(QAction *action = qobject_cast<QAction*>(sender())) {
		QModelIndexList list = selectedIndexes();

		if(RosterSortProxy *proxy = dynamic_cast<RosterSortProxy*>(model())) {
			if(RosterItemModel *itemModel = dynamic_cast<RosterItemModel*>(proxy->sourceModel())) {
				QString group = action->property("group").toString();
				if(action->property("group_new").toBool()) {
					group = QInputDialog::getText(this, tr("Add new group"), tr("Choose a name for the new group:"));
				}

				QList<RosterContact*> contacts;
				foreach(QModelIndex in, list) {
					QModelIndex index = proxy->mapToSource(in);

					if(index.isValid()) {
						if(index.data(RosterItem::TypeRole) == RosterItem::Contact) {
							if(RosterContact *cnt = static_cast<RosterContact*>(index.internalPointer())) {
								contacts.append(cnt);
							}
						}
					}
				}

				foreach(RosterContact *cnt, contacts) {
					itemModel->moveToGroup(cnt, group);
				}

				proxy->invalidate();
			}
		}
	}
}

void RosterTreeView::showVCard()
{
	QModelIndexList list = selectedIndexes();
	if(RosterSortProxy *proxy = dynamic_cast<RosterSortProxy*>(model())) {
		foreach(QModelIndex in, list) {
			QModelIndex index = proxy->mapToSource(in);
			if(index.data(RosterItem::TypeRole) == RosterItem::Contact) {
				if(RosterContact *cnt = static_cast<RosterContact*>(index.internalPointer())) {
					emit vCardRequested(cnt->contact());
				}
			}
		}
	}
}

void RosterTreeView::showHistory()
{
	QModelIndexList list = selectedIndexes();
	if(list.size() > 0) {
		QModelIndex index = dynamic_cast<RosterSortProxy*>(model())->mapToSource(list.first());
		if(index.data(RosterItem::TypeRole) == RosterItem::Contact) {
			if(RosterContact *cnt = static_cast<RosterContact*>(index.internalPointer())) {
				emit historyRequested(cnt->contact());
			}
		}
	}
}

void RosterTreeView::openURL()
{
	if(QAction *action = qobject_cast<QAction*>(sender())) {
		QDesktopServices::openUrl(action->text());
	}
}

void RosterTreeView::itemExpanded(const QModelIndex &index)
{
	if(index.data(RosterItem::TypeRole) == RosterItem::Group) {
		model()->setData(index, true, RosterItem::ExpandedRole);
	}
}

void RosterTreeView::itemCollapsed(const QModelIndex &index)
{
	if(index.data(RosterItem::TypeRole) == RosterItem::Group) {
		model()->setData(index, false, RosterItem::ExpandedRole);
	}
}

void RosterTreeView::setupBlinking(quint32 msgId, const KittySDK::IMessage &msg)
{
	if(msg.direction() == KittySDK::IMessage::Incoming) {
		if(!MessageQueue::inst()->isFirstForChat(msg.chat())) {
			return;
		}

		QTimer *blinkTimer = new QTimer(this);
		connect(blinkTimer, SIGNAL(timeout()), SLOT(blinkIcon()));
		blinkTimer->setProperty("account", msg.chat()->account()->uid());
		blinkTimer->setProperty("uid", msg.from()->uid());
		blinkTimer->start(Core::inst()->setting(KittySDK::Settings::S_BLINKING_SPEED, 500).toInt());

		m_blinkTimers.insert(msgId, blinkTimer);
	}
}

void RosterTreeView::blinkIcon()
{
	if(QTimer *timer = qobject_cast<QTimer*>(sender())) {
		QString account = timer->property("account").toString();
		QString uid = timer->property("uid").toString();

		if(RosterContact *cnt = rosterContact(account, uid)) {
			cnt->setData(!cnt->data(RosterContact::BlinkRole).toBool(), RosterContact::BlinkRole);

			viewport()->update();
		}
	}
}

void RosterTreeView::unblinkIcon(quint32 msgId)
{
	if(m_blinkTimers.contains(msgId)) {
		QTimer *timer = m_blinkTimers.take(msgId);
		timer->stop();

		QString account = timer->property("account").toString();
		QString uid = timer->property("uid").toString();

		if(RosterContact *cnt = rosterContact(account, uid)) {
			cnt->setData(false, RosterContact::BlinkRole);

			viewport()->update();
		}

		delete timer;
	}
}

void RosterTreeView::mousePressEvent(QMouseEvent *event)
{
	QTreeView::mousePressEvent(event);
	QModelIndex index = dynamic_cast<RosterSortProxy*>(model())->mapToSource(indexAt(event->pos()));

	if(index.isValid()) {
		if(event->button() == Qt::RightButton) {
			if(index.data(RosterItem::TypeRole).toInt() == RosterItem::Contact) {
				if(RosterContact *cnt = static_cast<RosterContact*>(index.internalPointer())) {
					QMenu menu(this);
					menu.addAction(Core::inst()->icon(KittySDK::Icons::I_MESSAGE), tr("Send message"), this, SLOT(sendMessage()));

					QString description = cnt->data(RosterItem::DescriptionRole).toString();

					QMenu urlMenu;
					if(!description.isEmpty()) {
						QRegExp urls("\\b(?:(?:https?|ftp):\\/\\/|www\\.|ftp\\.)[-A-Z0-9+&@#\\/%=~_|$?!:,.]*[A-Z0-9+&@#\\/%=~_|$]", Qt::CaseInsensitive);
						int pos = 0;
						while((pos = urls.indexIn(description, pos)) != -1) {
							QString url = urls.cap(0);

							urlMenu.addAction(url, this, SLOT(openURL()));

							pos += urls.matchedLength();
						}
					}

					if(urlMenu.actions().count()) {
						QAction *menuAction = menu.addMenu(&urlMenu);
						menuAction->setText(tr("URLs"));
						menuAction->setIcon(Core::inst()->icon(KittySDK::Icons::I_URL));
					}

					menu.addSeparator();

					menu.addAction(Core::inst()->icon(KittySDK::Icons::I_HISTORY), tr("History"), this, SLOT(showHistory()));
					menu.addAction(Core::inst()->icon(KittySDK::Icons::I_PROFILE), tr("vCard"), this, SLOT(showVCard()));
					menu.addSeparator();

					QMenu *copyMenu = menu.addMenu(Core::inst()->icon(KittySDK::Icons::I_COPY), tr("Copy"));
					copyMenu->addAction(tr("Name"), this, SLOT(copyName()));
					copyMenu->addAction(tr("UID"), this, SLOT(copyUid()));
					copyMenu->addAction(tr("Status description"), this, SLOT(copyDescription()));

					QMenu *groupMenu = menu.addMenu(Core::inst()->icon(KittySDK::Icons::I_GROUP), tr("Group"));
					QAction *noneAction = groupMenu->addAction(tr("None"), this, SLOT(moveToGroup()));
					noneAction->setProperty("group", "");
					if(cnt->contact()->group().isEmpty()) {
						noneAction->setCheckable(true);
						noneAction->setChecked(true);
					}

					groupMenu->addSeparator();

					// TODO:
					QStringList groups = ContactManager::inst()->groups();
					foreach(QString group, groups) {
						QAction *action = groupMenu->addAction(group, this, SLOT(moveToGroup()));
						action->setProperty("group", group);

						if(cnt->contact()->group() == group) {
							action->setCheckable(true);
							action->setChecked(true);
						}
					}

					if(groups.size() > 0) {
						groupMenu->addSeparator();
					}

					QAction *addAction = groupMenu->addAction(tr("Add new"), this, SLOT(moveToGroup()));
					addAction->setProperty("group_new", true);

					menu.addSeparator();

					KittySDK::IProtocol *proto = cnt->contact()->account()->protocol();
					if(proto) {
						if(proto->abilities() & KittySDK::IProtocol::BlockContacts) {
							menu.addAction(Core::inst()->icon(KittySDK::Icons::I_BLOCK), tr("Block"));
						}
					}

					menu.addAction(Core::inst()->icon(KittySDK::Icons::I_DELETE), tr("Remove"));

					cnt->contact()->prepareContextMenu(&menu);

					menu.exec(event->globalPos());
				}
			}
		}
	}
}

void RosterTreeView::mouseDoubleClickEvent(QMouseEvent *event)
{
	sendMessage();

	QTreeView::mouseDoubleClickEvent(event);
}

void RosterTreeView::keyPressEvent(QKeyEvent *event)
{
	if((event->key() == Qt::Key_Enter) || (event->key() == Qt::Key_Return)) {
		sendMessage();
	}

	QTreeView::keyPressEvent(event);
}

RosterContact *RosterTreeView::rosterContact(const QString &account, const QString &uid)
{
	for(int i = 0; i < model()->rowCount(); ++i) {
		QModelIndex idx = model()->index(i, 0, rootIndex());
		QModelIndex index = dynamic_cast<RosterSortProxy*>(model())->mapToSource(idx);

		if(index.data(RosterItem::TypeRole).toInt() == RosterItem::Contact) {
			if((index.data(RosterItem::AccountRole).toString() == account) && (index.data(RosterItem::UidRole).toString() == uid)) {
				if(RosterContact *cnt = static_cast<RosterContact*>(index.internalPointer())) {
					return cnt;
				}
			}
		} else if(index.data(RosterItem::TypeRole).toInt() == RosterItem::Group) {
			for(int j = 0; j < model()->rowCount(idx); ++j) {
				QModelIndex child = dynamic_cast<RosterSortProxy*>(model())->mapToSource(model()->index(j, 0, idx));

				if((child.data(RosterItem::AccountRole).toString() == account) && (child.data(RosterItem::UidRole).toString() == uid)) {
					if(RosterContact *cnt = static_cast<RosterContact*>(child.internalPointer())) {
						return cnt;
					}
				}
			}
		}
	}

	return 0;
}

}
