#include "RosterItem.h"

#include "SDK/constants.h"
#include "Core.h"

Kitty::RosterItem::RosterItem(RosterItem *parent): QStandardItem(), m_parent(parent)
{
  setData(Kitty::Core::inst()->icon(KittySDK::Icons::I_SMILEY), Qt::DecorationRole);
}

Kitty::RosterItem::~RosterItem()
{
  qDeleteAll(m_children);
}

void Kitty::RosterItem::appendChild(RosterItem *item)
{
  m_children.append(item);
}

Kitty::RosterItem *Kitty::RosterItem::child(int row)
{
  return m_children.value(row);
}

int Kitty::RosterItem::childCount() const
{
  return m_children.count();
}

int Kitty::RosterItem::columnCount() const
{
  return 1;
}

Kitty::RosterItem *Kitty::RosterItem::parent()
{
  return m_parent;
}

int Kitty::RosterItem::row() const
{
  if(m_parent) {
    return m_parent->m_children.indexOf(const_cast<Kitty::RosterItem*>(this));
  }

  return 0;
}
