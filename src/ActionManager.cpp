#include "ActionManager.h"


#include "widgets/windows/DebugWindow.h"
#include "SDK/constants.h"
#include "Core.h"

#include <QtCore/QDebug>
#include <QtGui/QApplication>

using namespace KittySDK;

void Kitty::ActionManager::loadDefaults()
{
  Core *core = Core::inst();

  insert(Actions::A_QUIT, new QAction(core->icon(Icons::I_QUIT), tr("Quit"), this));
  connect(action(Actions::A_QUIT), SIGNAL(triggered()), qApp, SLOT(quit()));

  insert(Actions::A_RESTART, new QAction(core->icon(Icons::I_REFRESH), tr("Restart"), this));
  connect(action(Actions::A_RESTART), SIGNAL(triggered()), core, SLOT(restart()));

  insert(Actions::A_PROFILE_FOLDER, new QAction(core->icon(Icons::I_FOLDER), tr("Profile folder"), this));
  insert(Actions::A_KITTY_FOLDER, new QAction(core->icon(Icons::I_FOLDER_KITTY), tr("Kitty's folder"), this));

  insert(Actions::A_ABOUT, new QAction(core->icon(Icons::I_INFO), tr("About..."), this));
  connect(action(Actions::A_ABOUT), SIGNAL(triggered()), core, SLOT(showAboutWindow()));

  insert(Actions::A_DEBUG, new QAction(core->icon(Icons::I_CONSOLE), tr("Debug console"), this));
  connect(action(Actions::A_DEBUG), SIGNAL(triggered()), DebugWindow::inst(), SLOT(show()));

  insert(Actions::A_SETTINGS, new QAction(core->icon(Icons::I_SETTINGS), tr("Settings"), this));
  connect(action(Actions::A_SETTINGS), SIGNAL(triggered()), core, SLOT(showSettingsWindow()));

  insert(Actions::A_SHOW_HIDE, new QAction(QIcon(), tr("Show / Hide"), this));
  connect(action(Actions::A_SHOW_HIDE), SIGNAL(triggered()), core, SLOT(toggleMainWindow()));
}
