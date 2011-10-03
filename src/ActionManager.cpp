#include "ActionManager.h"


#include "widgets/windows/DebugWindow.h"
#include "SDK/constants.h"
#include "Core.h"

#include <QtCore/QDebug>
#include <QtGui/QApplication>

#define qDebug() qDebug() << "[ActionManager]"
#define qWarning() qWarning() << "[ActionManager]"

using namespace Kitty;
using namespace KittySDK;

void Kitty::ActionManager::loadDefaults()
{
  qDebug() << "Loading default actions";

  Core *core = Core::inst();

  QAction *actQuit = new QAction(this);
  actQuit->setProperty("icon_id", Icons::I_QUIT);
  connect(actQuit, SIGNAL(triggered()), qApp, SLOT(quit()));
  insert(Actions::A_QUIT, actQuit);

  QAction *actRestart = new QAction(this);
  actRestart->setProperty("icon_id", Icons::I_REFRESH);
  connect(actRestart, SIGNAL(triggered()), core, SLOT(restart()));
  insert(Actions::A_RESTART, actRestart);

  QAction *actProfileFolder = new QAction(this);
  actProfileFolder->setProperty("icon_id", Icons::I_FOLDER);
  connect(actProfileFolder, SIGNAL(triggered()), core, SLOT(openProfilesFolder()));
  insert(Actions::A_PROFILE_FOLDER, actProfileFolder);

  QAction *actKittyFolder = new QAction(this);
  actKittyFolder->setProperty("icon_id", Icons::I_FOLDER_KITTY);
  connect(actKittyFolder, SIGNAL(triggered()), core, SLOT(openKittyFolder()));
  insert(Actions::A_KITTY_FOLDER, actKittyFolder);

  QAction *actAbout = new QAction(this);
  actAbout->setProperty("icon_id", Icons::I_INFO);
  connect(actAbout, SIGNAL(triggered()), core, SLOT(showAboutWindow()));
  insert(Actions::A_ABOUT, actAbout);

  QAction *actDebug = new QAction(this);
  actDebug->setProperty("icon_id", Icons::I_CONSOLE);
  connect(actDebug, SIGNAL(triggered()), DebugWindow::inst(), SLOT(show()));
  insert(Actions::A_DEBUG, actDebug);

  QAction *actSettings = new QAction(this);
  actSettings->setProperty("icon_id", Icons::I_SETTINGS);
  connect(actSettings, SIGNAL(triggered()), core, SLOT(showSettingsWindow()));
  insert(Actions::A_SETTINGS, actSettings);

  QAction *actShowHide = new QAction(this);
  connect(actShowHide, SIGNAL(triggered()), core, SLOT(toggleMainWindow()));
  insert(Actions::A_SHOW_HIDE, actShowHide);

  QAction *actAddContact = new QAction(this);
  actAddContact->setProperty("icon_id", Icons::I_USER);
  connect(actAddContact, SIGNAL(triggered()), core, SLOT(showAddContactWindow()));
  insert(Actions::A_ADD_CONTACT, actAddContact);

  QAction *actHistory = new QAction(this);
  actHistory->setProperty("icon_id", Icons::I_HISTORY);
  connect(actHistory, SIGNAL(triggered()), core, SLOT(showHistoryWindow()));
  insert(Actions::A_HISTORY, actHistory);

  QAction *actChangeProfile = new QAction(this);
  connect(actChangeProfile, SIGNAL(triggered()), core, SLOT(showProfilesWindow()));
  insert(Actions::A_CHANGE_PROFILE, actChangeProfile);

  updateIcons();
  retranslate();
}

void Kitty::ActionManager::updateIcons()
{
  qDebug() << "Updating icons";

  Core *core = Core::inst();

  QHashIterator<QString, QPointer<QAction> > it(m_actions);
  while(it.hasNext()) {
    it.next();

    if(!it.value()->property("icon_id").isNull()) {
      it.value()->setIcon(core->icon(it.value()->property("icon_id").toString()));
    }
  }
}

void Kitty::ActionManager::retranslate()
{
  action(Actions::A_QUIT)->setText(tr("Quit"));
  action(Actions::A_SHOW_HIDE)->setText(tr("Show / Hide"));
  action(Actions::A_SETTINGS)->setText(tr("Settings"));
  action(Actions::A_RESTART)->setText(tr("Restart"));
  action(Actions::A_KITTY_FOLDER)->setText(tr("Kitty's folder"));
  action(Actions::A_PROFILE_FOLDER)->setText(tr("Profile folder"));
  action(Actions::A_ABOUT)->setText(tr("About..."));
  action(Actions::A_DEBUG)->setText(tr("Debug console"));
  action(Actions::A_ADD_CONTACT)->setText(tr("Add contact"));
  action(Actions::A_HISTORY)->setText(tr("History"));
  action(Actions::A_CHANGE_PROFILE)->setText(tr("Change profile"));
}
