#include "RosterItem.h"

#include "Core.h"

#include <SDKConstants.h>

#define qDebug() qDebug() << "[RosterItem]"
#define qWarning() qWarning() << "[RosterItem]"

namespace Kitty
{

RosterItem::RosterItem(RosterItem *parent): QStandardItem(), m_parent(parent)
{

}

RosterItem::~RosterItem()
{
	qDeleteAll(m_children);
}

void RosterItem::appendChild(RosterItem *item)
{
	item->m_parent = this;
	m_children.append(item);
}

void RosterItem::removeChild(RosterItem *item)
{
	m_children.removeOne(item);
}

RosterItem *RosterItem::child(int row)
{
	return m_children.value(row);
}

int RosterItem::childCount() const
{
	return m_children.count();
}

RosterItem *RosterItem::parent()
{
	return m_parent;
}

int RosterItem::row() const
{
	if(m_parent) {
		if(RosterItem *item = const_cast<RosterItem*>(this)) {
			return m_parent->m_children.indexOf(item);
		}
	}

	return -1;
}

}
