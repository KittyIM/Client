#include "KittyActionMngr.h"
#include "KittyCore.h"
#include "wndDebug.h"
#include "SDK/constants.h"

#include <QtCore/QDebug>
#include <QtGui/QApplication>

using namespace KittySDK;

KittyActionMngr::KittyActionMngr(QObject *parent): QObject(parent)
{
}

void KittyActionMngr::loadDefaults()
{
  KittyCore *core = KittyCore::inst();

  insert(Actions::QUIT, new QAction(core->icon(Icons::QUIT), tr("Quit"), this));
  connect(action(Actions::QUIT), SIGNAL(triggered()), qApp, SLOT(quit()));

  insert(Actions::RESTART, new QAction(core->icon(Icons::REFRESH), tr("Restart"), this));
  connect(action(Actions::RESTART), SIGNAL(triggered()), core, SLOT(restart()));

  insert(Actions::OPEN_PROFILE_FOLDER, new QAction(core->icon(Icons::FOLDER), tr("Profile folder"), this));
  insert(Actions::OPEN_KITTY_FOLDER, new QAction(core->icon(Icons::FOLDER_KITTY), tr("Kitty's folder"), this));

  insert(Actions::ABOUT, new QAction(core->icon(Icons::INFO), tr("About..."), this));
  insert(Actions::DEBUG, new QAction(core->icon(Icons::CONSOLE), tr("Debug console"), this));
  connect(action(Actions::DEBUG), SIGNAL(triggered()), wndDebug::inst(), SLOT(show()));

  insert(Actions::SETTINGS, new QAction(core->icon(Icons::SETTINGS), tr("Settings"), this));
  insert(Actions::SHOW_HIDE, new QAction(QIcon(), tr("Show / Hide"), this));
  connect(action(Actions::SHOW_HIDE), SIGNAL(triggered()), core, SLOT(toggleMainWindow()));
}
