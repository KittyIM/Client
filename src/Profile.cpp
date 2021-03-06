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
		settings()->setValue(KittySDK::Settings::S_DEBUGWINDOW_GEOMETRY, m_core->debugWindow()->saveGeometry());

		m_core->messageQueue()->save(m_name);
		m_core->accountManager()->save(m_name);
		m_core->contactManager()->save(m_name);
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

	m_core->debugWindow()->restoreGeometry(m_core->setting(KittySDK::Settings::S_DEBUGWINDOW_GEOMETRY).toByteArray());
	m_core->actionManager()->loadDefaults();

	connect(m_core->iconManager(), SIGNAL(iconsUpdated()), m_core->actionManager(), SLOT(updateIcons()));

	m_core->pluginManager()->load();
	m_core->accountManager()->load(name);
	m_core->contactManager()->load(name);

	if(!m_core->setting(KittySDK::Settings::S_MAINWINDOW_STARTHIDDEN).toBool()) {
		m_core->mainWindow()->show();
	}

	//MessageQueue::inst()->load(name);

	//qDebug() << "Profile " + name + " loaded!";

	if(m_core->pluginManager()->hasError()) {
		PluginErrorWindow dlg(m_core);
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

		m_core->iconManager()->insert(i.key(), QPixmap(i.value()));
	}
}

}
