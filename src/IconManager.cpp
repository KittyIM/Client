#include "IconManager.h"

#include "SDK/constants.h"
#include "IconTheme.h"

#include <QtCore/QMap>
#include <QtCore/QDebug>

#define qDebug() qDebug() << "[IconManager]"
#define qWarning() qWarning() << "[IconManager]"

using namespace KittySDK;

Kitty::IconManager::IconManager(QObject *parent): QObject(parent)
{
  m_updateTimer.setSingleShot(true);
  m_updateTimer.setInterval(100);

  connect(&m_updateTimer, SIGNAL(timeout()), this, SIGNAL(iconsUpdated()));

  loadDefaults();
}

void Kitty::IconManager::loadDefaults()
{
  IconTheme theme;

  qDebug() << "Loading default icons [" << theme.icons().count() << "]";

  QHashIterator<QString, QString> i(theme.icons());
  while(i.hasNext()) {
    i.next();
    insert(i.key(), QPixmap(i.value()), false);
  }
}

void Kitty::IconManager::remove(const QString &id)
{
  m_icons.remove(id);
  emit iconsUpdated();
}

void Kitty::IconManager::insert(const QString &id, const QPixmap &pixmap, bool replace)
{
  if(m_icons.contains(id) && !replace) {
    return;
  }

  m_icons.insert(id, pixmap);

  m_updateTimer.start();
}
