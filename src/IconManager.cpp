#include "IconManager.h"

#include "IconTheme.h"

#include <SDKConstants.h>

#include <QtCore/QMap>
#include <QtCore/QDebug>

#define qDebug() qDebug() << "[IconManager]"
#define qWarning() qWarning() << "[IconManager]"

namespace Kitty
{

IconManager::IconManager(QObject *parent): QObject(parent)
{
	m_updateTimer.setSingleShot(true);
	m_updateTimer.setInterval(100);

	connect(&m_updateTimer, SIGNAL(timeout()), this, SIGNAL(iconsUpdated()));

	loadDefaults();
}

void IconManager::loadDefaults()
{
	IconTheme theme;

	qDebug() << "Loading default icons [" << theme.icons().count() << "]";

	QHashIterator<QString, QString> i(theme.icons());
	while(i.hasNext()) {
		i.next();
		insert(i.key(), QPixmap(i.value()), false);
	}
}

void IconManager::remove(const QString &id)
{
	m_icons.remove(id);
	emit iconsUpdated();
}

void IconManager::insert(const QString &id, const QPixmap &pixmap, bool replace)
{
	if(m_icons.contains(id) && !replace) {
		return;
	}

	m_icons.insert(id, pixmap);

	m_updateTimer.start();
}

}
