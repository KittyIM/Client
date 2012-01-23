#include "RosterItem.h"

#include "SDK/constants.h"
#include "Core.h"

#define qDebug() qDebug() << "[RosterItem]"
#define qWarning() qWarning() << "[RosterItem]"

using namespace Kitty;

Kitty::RosterItem::RosterItem(RosterItem *parent): QStandardItem(), m_parent(parent)
{

}

Kitty::RosterItem::~RosterItem()
{
	qDeleteAll(m_children);
}

void Kitty::RosterItem::appendChild(RosterItem *item)
{
	item->m_parent = this;
	m_children.append(item);
}

void Kitty::RosterItem::removeChild(RosterItem *item)
{
	m_children.removeAll(item);
}

RosterItem *Kitty::RosterItem::child(int row)
{
	return m_children.value(row);
}

int Kitty::RosterItem::childCount() const
{
	return m_children.count();
}

RosterItem *Kitty::RosterItem::parent()
{
	return m_parent;
}

int Kitty::RosterItem::row() const
{
	if(m_parent) {
		if(RosterItem *item = const_cast<Kitty::RosterItem*>(this)) {
			return m_parent->m_children.indexOf(item);
		}
	}

	return -1;
}
