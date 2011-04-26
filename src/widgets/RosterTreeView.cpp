#include "RosterTreeView.h"

#include "widgets/windows/SettingsWindow.h"
#include "RosterItemDelegate.h"
#include "RosterSortProxy.h"
#include "RosterItemModel.h"
#include "ContactManager.h"
#include "RosterContact.h"
#include "SDK/constants.h"
#include "SDK/Contact.h"
#include "RosterItem.h"
#include "Core.h"

#include <QtCore/QDebug>
#include <QtGui/QInputDialog>
#include <QtGui/QApplication>
#include <QtGui/QMouseEvent>
#include <QtGui/QClipboard>
#include <QtGui/QMenu>

using namespace Kitty;

Kitty::RosterTreeView::RosterTreeView(QWidget *parent): QTreeView(parent)
{
  m_delegate = new RosterItemDelegate(this);
  setItemDelegate(m_delegate);
  //setDragEnabled(true);
  //viewport()->setAcceptDrops(true);
  //setDropIndicatorShown(true);
  //setDragDropMode(QAbstractItemView::InternalMove);

  connect(this, SIGNAL(expanded(QModelIndex)), this, SLOT(itemExpanded(QModelIndex)));
  connect(this, SIGNAL(collapsed(QModelIndex)), this, SLOT(itemCollapsed(QModelIndex)));
}

void Kitty::RosterTreeView::fixGroups()
{
  QModelIndex root = rootIndex();
  for(int i = 0; i < model()->rowCount(root); i++) {
    QModelIndex index = model()->index(i, 0, root);
    if(index.data(RosterItem::TypeRole).toInt() == RosterItem::Group) {
      setExpanded(index, index.data(RosterItem::ExpandedRole).toBool());
    }
  }
}

void Kitty::RosterTreeView::copyName()
{
  QModelIndexList list = selectedIndexes();
  RosterSortProxy* proxy = static_cast<RosterSortProxy*>(model());
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

void Kitty::RosterTreeView::copyUid()
{
  QModelIndexList list = selectedIndexes();
  RosterSortProxy* proxy = static_cast<RosterSortProxy*>(model());
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

void Kitty::RosterTreeView::copyDescription()
{
  QModelIndexList list = selectedIndexes();
  if(list.size() > 0) {
    QModelIndex index = static_cast<RosterSortProxy*>(model())->mapToSource(list.first());
    qApp->clipboard()->setText(index.data(RosterItem::DescriptionRole).toString());
  }
}

void Kitty::RosterTreeView::moveToGroup()
{
  QAction *action = dynamic_cast<QAction*>(sender());
  QModelIndexList list = selectedIndexes();

  // TODO
  if(action && (list.size() > 0)) {
    RosterSortProxy *proxy = static_cast<RosterSortProxy*>(model());
    RosterItemModel *itemModel = static_cast<RosterItemModel*>(proxy->sourceModel());

    QString group = action->property("group").toString();
    if(action->property("group_new").toBool()) {
      group = QInputDialog::getText(this, tr("Add new group"), tr("Choose a name for the new group:"));
    }

    QModelIndex index = proxy->mapToSource(list.first());
    RosterContact *cnt = static_cast<RosterContact*>(index.internalPointer());

    itemModel->moveToGroup(cnt, group);

    proxy->invalidate();
  }
}

void Kitty::RosterTreeView::itemExpanded(const QModelIndex &index)
{
  if(index.data(RosterItem::TypeRole) == RosterItem::Group) {
    model()->setData(index, true, RosterItem::ExpandedRole);
  }
}

void Kitty::RosterTreeView::itemCollapsed(const QModelIndex &index)
{
  if(index.data(RosterItem::TypeRole) == RosterItem::Group) {
    model()->setData(index, false, RosterItem::ExpandedRole);
  }
}

void Kitty::RosterTreeView::mousePressEvent(QMouseEvent *event)
{
  QTreeView::mousePressEvent(event);
  QModelIndex index = static_cast<RosterSortProxy*>(model())->mapToSource(indexAt(event->pos()));

  if(index.isValid()) {
    if(event->button() == Qt::RightButton) {
      if(index.data(RosterItem::TypeRole).toInt() == RosterItem::Contact) {
        RosterContact *cnt = static_cast<RosterContact*>(index.internalPointer());
        if(cnt) {
          QMenu menu(this);
          menu.addAction(Core::inst()->icon(KittySDK::Icons::I_MESSAGE), tr("Send message"));
          menu.addSeparator();

          menu.addAction(Core::inst()->icon(KittySDK::Icons::I_HISTORY), tr("History"));
          menu.addAction(Core::inst()->icon(KittySDK::Icons::I_PROFILE), tr("vCard"));
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

          // TODO
          QStringList groups;
          foreach(KittySDK::Contact *contact, ContactManager::inst()->contacts()) {
            if(!contact->group().isEmpty() && !groups.contains(contact->group())) {
              QAction *action = groupMenu->addAction(contact->group(), this, SLOT(moveToGroup()));
              action->setProperty("group", contact->group());

              if(contact->group() == cnt->contact()->group()) {
                action->setCheckable(true);
                action->setChecked(true);
              }

              groups.append(contact->group());
            }
          }

          if(groups.size() > 0) {
            groupMenu->addSeparator();
          }

          QAction *addAction = groupMenu->addAction(tr("Add new"), this, SLOT(moveToGroup()));
          addAction->setProperty("group_new", true);

          menu.addSeparator();

          KittySDK::Protocol *proto = cnt->contact()->account()->protocol();
          if(proto) {
            if(proto->abilities().testFlag(KittySDK::Protocol::BlockContacts)) {
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
