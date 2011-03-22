#include "Profile.h"

#include "widgets/windows/SettingsWindow.h"
#include "widgets/windows/DebugWindow.h"
#include "widgets/windows/ChatWindow.h"
#include "widgets/windows/MainWindow.h"
#include "SDK/constants.h"
#include "ActionManager.h"
#include "PluginManager.h"
#include "IconManager.h"
#include "XmlSettings.h"
#include "IconTheme.h"
#include "Core.h"
#include "App.h"

#include <QtCore/QLibraryInfo>
#include <QtCore/QTranslator>
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
  if(isLoaded()) {
    settings()->setValue(Settings::S_DEBUGWINDOW_GEOMETRY, DebugWindow::inst()->saveGeometry());
  }
}

void Kitty::Profile::load(const QString &name)
{
  Kitty::Core *core = Kitty::Core::inst();

  m_name = name;
  m_settings = new XmlSettings(core->profilesDir() + name + "/settings.xml", this);

  if(m_settings->contains(Settings::S_ICON_THEME)) {
    loadIconTheme(settings()->value(Settings::S_ICON_THEME).toString());
  }

  static_cast<Kitty::App*>(qApp)->applySettings();

  ActionManager::inst()->loadDefaults();

  connect(IconManager::inst(), SIGNAL(iconsUpdated()), ActionManager::inst(), SLOT(updateIcons()));

  PluginManager::inst()->load();

  core->mainWindow();

  qDebug() << "Profile " + name + " loaded!";
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
