#include "Profile.h"

#include "widgets/DebugWindow.h"
#include "SDK/constants.h"
#include "IconManager.h"
#include "XmlSettings.h"
#include "IconTheme.h"
#include "Core.h"

#include <QtCore/QDebug>
#include <QtCore/QFile>
#include <QtCore/QDir>
#include <QtGui/QApplication>
#include <QtXml/QDomDocument>
#include <QtXml/QDomElement>

using namespace KittySDK;

Kitty::Profile::Profile(QObject *parent): QObject(parent)
{
  m_settings = 0;
}

Kitty::Profile::~Profile()
{
  if(isLoaded() && !m_settingsOnly) {
    settings()->setValue(Settings::S_DEBUGWINDOW_GEOMETRY, DebugWindow::inst()->saveGeometry());
  }
}

void Kitty::Profile::load(const QString &name, bool settingsOnly)
{
  Core *core = Core::inst();

  m_settings = new XmlSettings(core->profilesDir() + name + "/settings.xml", this);

  m_settingsOnly = settingsOnly;
  if(!m_settingsOnly) {
    if(settings()->contains(Settings::S_PROFILE_THEMES_ICON)) {
      loadIconTheme(settings()->value(Settings::S_PROFILE_THEMES_ICON).toString());
    }
  }

  m_name = name;
}

bool Kitty::Profile::hasPassword() const
{
  return !settings()->value(Settings::S_PROFILE_PASSWORD).toString().isEmpty();
}

void Kitty::Profile::loadIconTheme(const QString &name)
{
  qDebug() << "Loading icon theme " + name;

  IconTheme theme(name);
  QMapIterator<QString, QString> i(theme.icons());
  while(i.hasNext()) {
    i.next();

    IconManager::inst()->insert(i.key(), QPixmap(i.value()));
  }
}
