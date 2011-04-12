#include "RosterTreeView.h"

#include "widgets/windows/SettingsWindow.h"
#include "RosterItemDelegate.h"
#include "ContactManager.h"
#include "RosterContact.h"
#include "SDK/constants.h"
#include "SDK/Contact.h"
#include "Core.h"

#include <QtCore/QDebug>
#include <QtGui/QMouseEvent>
#include <QtGui/QMenu>

Kitty::RosterTreeView::RosterTreeView(QWidget *parent): QTreeView(parent)
{
  m_delegate = new Kitty::RosterItemDelegate(this);

  setItemDelegate(m_delegate);
}

void Kitty::RosterTreeView::mousePressEvent(QMouseEvent *event)
{
  QTreeView::mousePressEvent(event);

  QModelIndex index = indexAt(event->pos());

  if(index.isValid()) {
    if(event->button() == Qt::RightButton) {
      if(index.data(RosterItem::TypeRole).toInt() == Kitty::RosterItem::Contact) {
        Kitty::RosterContact *cnt = static_cast<Kitty::RosterContact*>(index.internalPointer());
        if(cnt) {
          QMenu menu(this);
          menu.addAction(Kitty::Core::inst()->icon(KittySDK::Icons::I_MESSAGE), tr("Send message"));
          menu.addSeparator();
          menu.addAction(Kitty::Core::inst()->icon(KittySDK::Icons::I_HISTORY), tr("History"));
          menu.addAction(Kitty::Core::inst()->icon(KittySDK::Icons::I_PROFILE), tr("vCard"));
          menu.addSeparator();

          QMenu *copyMenu = menu.addMenu(Kitty::Core::inst()->icon(KittySDK::Icons::I_COPY), tr("Copy"));
          copyMenu->addAction(tr("Name"));
          copyMenu->addAction(tr("UID"));
          copyMenu->addAction(tr("Status description"));

          QMenu *groupMenu = menu.addMenu(Kitty::Core::inst()->icon(KittySDK::Icons::I_GROUP), tr("Group"));
          groupMenu->addAction(tr("None"));
          groupMenu->addSeparator();

          QStringList groups;
          foreach(KittySDK::Contact *cnt, Kitty::ContactManager::inst()->contacts()) {
            if(!groups.contains(cnt->group())) {
              groupMenu->addAction(cnt->group());
              groups.append(cnt->group());
            }
          }

          menu.addSeparator();

          if(cnt->contact()->account()->protocol()->abilities().testFlag(KittySDK::Protocol::BlockContacts)) {
            menu.addAction(Kitty::Core::inst()->icon(KittySDK::Icons::I_BLOCK), tr("Block"));
          }

          menu.addAction(Kitty::Core::inst()->icon(KittySDK::Icons::I_DELETE), tr("Remove"));

          cnt->contact()->prepareContextMenu(&menu);

          menu.exec(event->globalPos());
        }
      }
    }
  }
}
