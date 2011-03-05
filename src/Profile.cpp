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

using namespace Kitty;
using namespace KittySDK;

Profile::Profile(QObject *parent): QObject(parent)
{
  m_settings = 0;
}

Profile::~Profile()
{
  if(isLoaded()) {
    settings()->setValue(Settings::S_DEBUGWINDOW_GEOMETRY, DebugWindow::inst()->saveGeometry());
  }
}

void Profile::load(const QString &name, bool settingsOnly)
{
  Core *core = Core::inst();

  m_settings = new XmlSettings(core->profilesDir() + name + "/settings.xml", this);

  if(!settingsOnly) {
    if(settings()->contains(Settings::S_PROFILE_THEMES_ICON)) {
      loadIconTheme(settings()->value(Settings::S_PROFILE_THEMES_ICON).toString());
    }
  }

  m_name = name;
}

bool Profile::hasPassword()
{
  return !settings()->value(Settings::S_PROFILE_PASSWORD).toString().isEmpty();
}

void Profile::loadIconTheme(const QString &name)
{
  qDebug() << "Loading icon theme " + name;

  Core *core = Core::inst();

  IconTheme theme(name);
  QMapIterator<QString, QString> i(theme.icons());
  while(i.hasNext()) {
    i.next();

    core->iconManager()->insert(i.key(), QPixmap(i.value()));
  }
}
