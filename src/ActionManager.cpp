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

  QAction *actQuit = new QAction(tr("Quit"), this);
  actQuit->setProperty("icon_id", Icons::I_QUIT);
  connect(actQuit, SIGNAL(triggered()), qApp, SLOT(quit()));
  insert(Actions::A_QUIT, actQuit);

  QAction *actRestart = new QAction(tr("Restart"), this);
  actRestart->setProperty("icon_id", Icons::I_REFRESH);
  connect(actRestart, SIGNAL(triggered()), core, SLOT(restart()));
  insert(Actions::A_RESTART, actRestart);

  QAction *actProfileFolder = new QAction(tr("Profile folder"), this);
  actProfileFolder->setProperty("icon_id", Icons::I_FOLDER);
  connect(actProfileFolder, SIGNAL(triggered()), core, SLOT(openProfilesFolder()));
  insert(Actions::A_PROFILE_FOLDER, actProfileFolder);

  QAction *actKittyFolder = new QAction(tr("Kitty's folder"), this);
  actKittyFolder->setProperty("icon_id", Icons::I_FOLDER_KITTY);
  connect(actKittyFolder, SIGNAL(triggered()), core, SLOT(openKittyFolder()));
  insert(Actions::A_KITTY_FOLDER, actKittyFolder);

  QAction *actAbout = new QAction(tr("About..."), this);
  actAbout->setProperty("icon_id", Icons::I_INFO);
  connect(actAbout, SIGNAL(triggered()), core, SLOT(showAboutWindow()));
  insert(Actions::A_ABOUT, actAbout);

  QAction *actDebug = new QAction(tr("Debug console"), this);
  actDebug->setProperty("icon_id", Icons::I_CONSOLE);
  connect(actDebug, SIGNAL(triggered()), DebugWindow::inst(), SLOT(show()));
  insert(Actions::A_DEBUG, actDebug);

  QAction *actSettings = new QAction(tr("Settings"), this);
  actSettings->setProperty("icon_id", Icons::I_SETTINGS);
  connect(actSettings, SIGNAL(triggered()), core, SLOT(showSettingsWindow()));
  insert(Actions::A_SETTINGS, actSettings);

  QAction *actShowHide = new QAction(tr("Show / Hide"), this);
  connect(actShowHide, SIGNAL(triggered()), core, SLOT(toggleMainWindow()));
  insert(Actions::A_SHOW_HIDE, actShowHide);

  QAction *actAddContact = new QAction(tr("Add contact"), this);
  actAddContact->setProperty("icon_id", Icons::I_USER);
  connect(actAddContact, SIGNAL(triggered()), core, SLOT(showAddContactWindow()));
  insert(Actions::A_ADD_CONTACT, actAddContact);

  QAction *actHistory = new QAction(tr("History"), this);
  actHistory->setProperty("icon_id", Icons::I_HISTORY);
  connect(actHistory, SIGNAL(triggered()), core, SLOT(showHistoryWindow()));
  insert(Actions::A_HISTORY, actHistory);

  updateIcons();
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
    } else {
      qWarning() << "  " << it.value()->text() << " has no icon_id";
    }
  }
}
