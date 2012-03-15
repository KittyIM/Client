#include "ActionManager.h"

#include "widgets/windows/DebugWindow.h"
#include "Core.h"

#include <SDKConstants.h>

#include <QtCore/QDebug>
#include <QtGui/QApplication>

#define qDebug() qDebug() << "[ActionManager]"
#define qWarning() qWarning() << "[ActionManager]"

namespace Kitty
{

ActionManager::ActionManager(Core *core):
	QObject(core),
	m_core(core)
{
}

ActionManager::~ActionManager()
{
	qDeleteAll(m_actions);
}

void ActionManager::loadDefaults()
{
	//qDebug() << "Loading default actions";
	QAction *actQuit = new QAction(this);
	actQuit->setProperty("icon_id", KittySDK::Icons::I_QUIT);
	connect(actQuit, SIGNAL(triggered()), qApp, SLOT(quit()));
	insert(KittySDK::Actions::A_QUIT, actQuit);

	QAction *actRestart = new QAction(this);
	actRestart->setProperty("icon_id", KittySDK::Icons::I_REFRESH);
	connect(actRestart, SIGNAL(triggered()), m_core, SLOT(restart()));
	insert(KittySDK::Actions::A_RESTART, actRestart);

	QAction *actProfileFolder = new QAction(this);
	actProfileFolder->setProperty("icon_id", KittySDK::Icons::I_FOLDER);
	connect(actProfileFolder, SIGNAL(triggered()), m_core, SLOT(openProfilesFolder()));
	insert(KittySDK::Actions::A_PROFILE_FOLDER, actProfileFolder);

	QAction *actKittyFolder = new QAction(this);
	actKittyFolder->setProperty("icon_id", KittySDK::Icons::I_FOLDER_KITTY);
	connect(actKittyFolder, SIGNAL(triggered()), m_core, SLOT(openKittyFolder()));
	insert(KittySDK::Actions::A_KITTY_FOLDER, actKittyFolder);

	QAction *actAbout = new QAction(this);
	actAbout->setProperty("icon_id", KittySDK::Icons::I_INFO);
	connect(actAbout, SIGNAL(triggered()), m_core, SLOT(showAboutWindow()));
	insert(KittySDK::Actions::A_ABOUT, actAbout);

	QAction *actDebug = new QAction(this);
	actDebug->setProperty("icon_id", KittySDK::Icons::I_CONSOLE);
	connect(actDebug, SIGNAL(triggered()), m_core->debugWindow(), SLOT(show()));
	insert(KittySDK::Actions::A_DEBUG, actDebug);

	QAction *actSettings = new QAction(this);
	actSettings->setProperty("icon_id", KittySDK::Icons::I_SETTINGS);
	connect(actSettings, SIGNAL(triggered()), m_core, SLOT(showSettingsWindow()));
	insert(KittySDK::Actions::A_SETTINGS, actSettings);

	QAction *actShowHide = new QAction(this);
	connect(actShowHide, SIGNAL(triggered()), m_core, SLOT(toggleMainWindow()));
	insert(KittySDK::Actions::A_SHOW_HIDE, actShowHide);

	QAction *actAddContact = new QAction(this);
	actAddContact->setProperty("icon_id", KittySDK::Icons::I_USER);
	connect(actAddContact, SIGNAL(triggered()), m_core, SLOT(showAddContactWindow()));
	insert(KittySDK::Actions::A_ADD_CONTACT, actAddContact);

	QAction *actHistory = new QAction(this);
	actHistory->setProperty("icon_id", KittySDK::Icons::I_HISTORY);
	connect(actHistory, SIGNAL(triggered()), m_core, SLOT(showHistoryWindow()));
	insert(KittySDK::Actions::A_HISTORY, actHistory);

	QAction *actChangeProfile = new QAction(this);
	connect(actChangeProfile, SIGNAL(triggered()), m_core, SLOT(showProfilesWindow()));
	insert(KittySDK::Actions::A_CHANGE_PROFILE, actChangeProfile);

	updateIcons();
	retranslate();
}

void ActionManager::insert(const QString &id, QAction *action)
{
	if(!m_actions.contains(id)) {
		m_actions.insert(id, action);
	}
}

void ActionManager::updateIcons()
{
	//qDebug() << "Updating icons";

	QHashIterator<QString, QPointer<QAction> > it(m_actions);
	while(it.hasNext()) {
		it.next();

		if(!it.value()->property("icon_id").isNull()) {
			it.value()->setIcon(m_core->icon(it.value()->property("icon_id").toString()));
		}
	}
}

void ActionManager::retranslate()
{
	action(KittySDK::Actions::A_QUIT)->setText(tr("Quit"));
	action(KittySDK::Actions::A_SHOW_HIDE)->setText(tr("Show / Hide"));
	action(KittySDK::Actions::A_SETTINGS)->setText(tr("Settings"));
	action(KittySDK::Actions::A_RESTART)->setText(tr("Restart"));
	action(KittySDK::Actions::A_KITTY_FOLDER)->setText(tr("Kitty's folder"));
	action(KittySDK::Actions::A_PROFILE_FOLDER)->setText(tr("Profile folder"));
	action(KittySDK::Actions::A_ABOUT)->setText(tr("About..."));
	action(KittySDK::Actions::A_DEBUG)->setText(tr("Debug console"));
	action(KittySDK::Actions::A_ADD_CONTACT)->setText(tr("Add contact"));
	action(KittySDK::Actions::A_HISTORY)->setText(tr("History"));
	action(KittySDK::Actions::A_CHANGE_PROFILE)->setText(tr("Change profile"));
}

}
