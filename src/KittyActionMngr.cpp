#include "KittyActionMngr.h"
#include "KittyCore.h"
#include "SDK/constants.h"

#include <QtGui/QAction>
#include <QtGui/QApplication>

using namespace KittySDK;

KittyActionMngr::KittyActionMngr(QObject *parent): QObject(parent)
{
}

void KittyActionMngr::loadDefaults()
{
  KittyCore *core = KittyCore::inst();

  addAction(Actions::QUIT, new QAction(core->getIcon(Icons::QUIT), tr("Quit"), this));
  connect(getAction(Actions::QUIT), SIGNAL(triggered()), qApp, SLOT(quit()));

  addAction(Actions::RESTART, new QAction(core->getIcon(Icons::REFRESH), tr("Restart"), this));
  addAction(Actions::OPEN_PROFILE_FOLDER, new QAction(core->getIcon(Icons::FOLDER), tr("Profile folder"), this));
  addAction(Actions::OPEN_KITTY_FOLDER, new QAction(core->getIcon(Icons::FOLDER_KITTY), tr("Kitty's folder"), this));

  addAction(Actions::SETTINGS, new QAction(core->getIcon(Icons::SETTINGS), tr("Settings"), this));
  addAction(Actions::SHOW_HIDE, new QAction(QIcon(), tr("Show / Hide"), this));
}

void KittyActionMngr::addAction(const int &id, QAction *action)
{
  m_actions.insert(id, action);
}

QAction *KittyActionMngr::getAction(const int &id)
{
  return m_actions.value(id);
}
