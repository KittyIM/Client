#include "RosterSortProxy.h"

#include "ContactManager.h"
#include "RosterContact.h"
#include "Core.h"

#include <SDKConstants.h>
#include <IProtocol.h>
#include <IContact.h>

#include <QtCore/QDebug>

#define qDebug() qDebug() << "[RosterSortProxy]"
#define qWarning() qWarning() << "[RosterSortProxy]"

namespace Kitty
{

RosterSortProxy::RosterSortProxy(QObject *parent): QSortFilterProxyModel(parent)
{
	setDynamicSortFilter(true);
	setFilterCaseSensitivity(Qt::CaseInsensitive);

	connect(ContactManager::inst(), SIGNAL(statusUpdated()), SLOT(invalidate()));
}

bool RosterSortProxy::lessThan(const QModelIndex &left, const QModelIndex &right) const
{
	int leftType = left.data(RosterItem::TypeRole).toInt();
	int rightType = right.data(RosterItem::TypeRole).toInt();

	if((leftType == RosterItem::Contact) && (rightType == RosterItem::Contact)) {
		if(left.data(RosterItem::TemporaryRole).toBool() && !right.data(RosterItem::TemporaryRole).toBool()) {
			 return true;
		} else if(!left.data(RosterItem::TemporaryRole).toBool() && right.data(RosterItem::TemporaryRole).toBool()) {
			 return false;
		} else if(left.data(RosterItem::StatusRole).toInt() == right.data(RosterItem::StatusRole).toInt()) {
			return QString::localeAwareCompare(left.data().toString(), right.data().toString()) > 0;
		} else if((left.data(RosterItem::StatusRole).toInt() == right.data(RosterItem::StatusRole).toInt()) ||
				  (left.data(RosterItem::TemporaryRole).toBool() && right.data(RosterItem::TemporaryRole).toBool())) {
			return QString::localeAwareCompare(left.data().toString(), right.data().toString()) > 0;
		} else {
			 return left.data(RosterItem::StatusRole).toInt() > right.data(RosterItem::StatusRole).toInt();
		 }
	} else if((leftType == RosterItem::Group) && (rightType == RosterItem::Group)) {
		return QString::localeAwareCompare(left.data().toString(), right.data().toString()) > 0;
	} else {
		if((leftType == RosterItem::Contact) && (rightType == RosterItem::Group)) {
			return true;
		} else {
			return false;
		}
	}

	return true;
}

bool RosterSortProxy::filterAcceptsRow(int source_row, const QModelIndex& source_parent) const
{
	QModelIndex index = sourceModel()->index(source_row, 0, source_parent);

	if(index.data(RosterItem::TypeRole).toInt() == RosterItem::Group) {
		return true;
	}

	if(!m_groupFilter.isEmpty()) {
		return (index.parent().data() == m_groupFilter);
	}

	if(!filterRegExp().isEmpty()) {
		return QSortFilterProxyModel::filterAcceptsRow(source_row, source_parent);
	}

	if(Core::inst()->setting(KittySDK::Settings::S_ROSTER_HIDE_OFFLINE, false).toBool()) {
		return index.data(RosterItem::StatusRole).toInt() < KittySDK::IProtocol::Offline;
	}

	return true;
}

}
