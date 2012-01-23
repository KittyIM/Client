#include "Profile.h"

#include "widgets/windows/SettingsWindow.h"
#include "widgets/windows/DebugWindow.h"
#include "widgets/windows/ChatWindow.h"
#include "widgets/windows/MainWindow.h"
#include "ContactManager.h"
#include "AccountManager.h"
#include "SDK/constants.h"
#include "ActionManager.h"
#include "PluginManager.h"
#include "IconManager.h"
#include "JsonSettings.h"
#include "ChatTheme.h"
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

#define qDebug() qDebug() << "[Profile]"
#define qWarning() qWarning() << "[Profile]"

using namespace Kitty;
using namespace KittySDK;

Kitty::Profile::Profile(QObject *parent): QObject(parent)
{
	m_settings = 0;
}

Kitty::Profile::~Profile()
{
	if(isLoaded()) {
		settings()->setValue(Settings::S_DEBUGWINDOW_GEOMETRY, DebugWindow::inst()->saveGeometry());

		AccountManager::inst()->save(m_name);
		ContactManager::inst()->save(m_name);
	}
}

void Kitty::Profile::load(const QString &name)
{
	Core *core = Core::inst();

	m_name = name;
	m_settings = new JsonSettings(core->profilesDir() + name + "/settings.dat", this);

	if(m_settings->contains(Settings::S_ICON_THEME)) {
		loadIconTheme(settings()->value(Settings::S_ICON_THEME).toString());
	}

	dynamic_cast<App*>(qApp)->applySettings();

	ActionManager::inst()->loadDefaults();

	connect(IconManager::inst(), SIGNAL(iconsUpdated()), ActionManager::inst(), SLOT(updateIcons()));

	PluginManager::inst()->load();

	AccountManager::inst()->load(name);

	ContactManager::inst()->load(name);

	qDebug() << "Profile " + name + " loaded!";
}

void Kitty::Profile::loadIconTheme(const QString &name)
{
	qDebug() << "Loading icon theme " + name;

	IconTheme theme(name);
	QHashIterator<QString, QString> i(theme.icons());
	while(i.hasNext()) {
		i.next();

		IconManager::inst()->insert(i.key(), QPixmap(i.value()));
	}
}
