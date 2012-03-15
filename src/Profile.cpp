#include "Profile.h"

#include "widgets/windows/PluginErrorWindow.h"
#include "widgets/windows/SettingsWindow.h"
#include "widgets/windows/DebugWindow.h"
#include "widgets/windows/ChatWindow.h"
#include "widgets/windows/MainWindow.h"
#include "ContactManager.h"
#include "AccountManager.h"
#include "ActionManager.h"
#include "PluginManager.h"
#include "MessageQueue.h"
#include "JsonSettings.h"
#include "IconManager.h"
#include "ChatTheme.h"
#include "IconTheme.h"
#include "Core.h"
#include "App.h"

#include <SDKConstants.h>

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

namespace Kitty
{

Profile::Profile(Core *core):
	QObject(core),
	m_core(core)
{
	m_settings = 0;
}

Profile::~Profile()
{
	if(isLoaded()) {
		settings()->setValue(KittySDK::Settings::S_DEBUGWINDOW_GEOMETRY, DebugWindow::inst()->saveGeometry());

		MessageQueue::inst()->save(m_name);
		m_core->accountManager()->save(m_name);
		ContactManager::inst()->save(m_name);
	}
}

void Profile::load(const QString &name)
{
	m_name = name;
	m_settings = new JsonSettings(m_core->profilesDir() + name + "/settings.dat", this);

	if(m_settings->contains(KittySDK::Settings::S_ICON_THEME)) {
		loadIconTheme(settings()->value(KittySDK::Settings::S_ICON_THEME).toString());
	}

	dynamic_cast<App*>(qApp)->retranslate();
	dynamic_cast<App*>(qApp)->applySettings();

	DebugWindow::inst()->restoreGeometry(m_core->setting(KittySDK::Settings::S_DEBUGWINDOW_GEOMETRY).toByteArray());

	ActionManager::inst()->loadDefaults();

	connect(IconManager::inst(), SIGNAL(iconsUpdated()), ActionManager::inst(), SLOT(updateIcons()));

	PluginManager::inst()->load();

	m_core->accountManager()->load(name);

	ContactManager::inst()->load(name);

	if(!m_core->setting(KittySDK::Settings::S_MAINWINDOW_STARTHIDDEN).toBool()) {
		m_core->mainWindow()->show();
	}

	//MessageQueue::inst()->load(name);

	//qDebug() << "Profile " + name + " loaded!";

	if(PluginManager::inst()->hasError()) {
		PluginErrorWindow dlg;
		dlg.exec();
	}
}

void Profile::loadIconTheme(const QString &name)
{
	//qDebug() << "Loading icon theme " + name;

	IconTheme theme(name);
	QHashIterator<QString, QString> i(theme.icons());
	while(i.hasNext()) {
		i.next();

		IconManager::inst()->insert(i.key(), QPixmap(i.value()));
	}
}

}
