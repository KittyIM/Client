#include "RosterTreeView.h"

#include "widgets/windows/SettingsWindow.h"
#include "RosterItemDelegate.h"
#include "ContactManager.h"
#include "RosterContact.h"
#include "SDK/constants.h"
#include "SDK/Contact.h"
#include "RosterItem.h"
#include "Core.h"

#include <QtCore/QDebug>
#include <QtGui/QMouseEvent>
#include <QtGui/QMenu>

using namespace Kitty;

Kitty::RosterTreeView::RosterTreeView(QWidget *parent): QTreeView(parent)
{
  m_delegate = new RosterItemDelegate(this);
  setItemDelegate(m_delegate);

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

  QModelIndex index = indexAt(event->pos());

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
          copyMenu->addAction(tr("Name"));
          copyMenu->addAction(tr("UID"));
          copyMenu->addAction(tr("Status description"));

          QMenu *groupMenu = menu.addMenu(Core::inst()->icon(KittySDK::Icons::I_GROUP), tr("Group"));
          groupMenu->addAction(tr("None"));
          groupMenu->addSeparator();

          QStringList groups;
          foreach(KittySDK::Contact *cnt, ContactManager::inst()->contacts()) {
            if(!groups.contains(cnt->group())) {
              groupMenu->addAction(cnt->group());
              groups.append(cnt->group());
            }
          }

          menu.addSeparator();

          if(cnt->contact()->account()->protocol()->abilities().testFlag(KittySDK::Protocol::BlockContacts)) {
            menu.addAction(Core::inst()->icon(KittySDK::Icons::I_BLOCK), tr("Block"));
          }

          menu.addAction(Core::inst()->icon(KittySDK::Icons::I_DELETE), tr("Remove"));

          cnt->contact()->prepareContextMenu(&menu);

          menu.exec(event->globalPos());
        }
      }
    }
  }
}
