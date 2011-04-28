#include "RosterSortProxy.h"

#include "ContactManager.h"
#include "RosterContact.h"
#include "SDK/constants.h"
#include "SDK/Protocol.h"
#include "SDK/Contact.h"
#include "Core.h"

#include <QtCore/QDebug>

using namespace Kitty;
using namespace KittySDK;

Kitty::RosterSortProxy::RosterSortProxy(QObject *parent): QSortFilterProxyModel(parent)
{
  setDynamicSortFilter(true);

  connect(ContactManager::inst(), SIGNAL(statusUpdated()), this, SLOT(invalidate()));
}

bool Kitty::RosterSortProxy::lessThan(const QModelIndex &left, const QModelIndex &right) const
{
  int type = left.data(RosterItem::TypeRole).toInt();

  if(type == RosterItem::Contact) {
    if(left.data(RosterItem::StatusRole).toInt() == right.data(RosterItem::StatusRole).toInt()) {
      return QString::localeAwareCompare(left.data().toString(), right.data().toString()) > 0;
    } else {
      return left.data(RosterItem::StatusRole).toInt() > right.data(RosterItem::StatusRole).toInt();
    }
  } else if(type == RosterItem::Group) {
    return QString::localeAwareCompare(left.data().toString(), right.data().toString()) > 0;
  }

  return true;
}

bool Kitty::RosterSortProxy::filterAcceptsRow(int source_row, const QModelIndex& source_parent) const
{
  QModelIndex index = sourceModel()->index(source_row, 0, source_parent);

  if(Core::inst()->setting(Settings::S_ROSTER_HIDE_OFFLINE, false).toBool()) {
    if(index.data(RosterItem::TypeRole).toInt() == RosterItem::Group) {
      return true;
    } else {
      return index.data(RosterItem::StatusRole).toInt() < Protocol::Offline;
    }
  } else {
    return true;
  }
}
