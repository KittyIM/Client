#include "IconManager.h"

#include "SDK/constants.h"

#include <QtCore/QMap>

using namespace Kitty;
using namespace KittySDK;

IconManager::IconManager(QObject *parent): QObject(parent)
{
  loadDefaults();
}

void IconManager::loadDefaults()
{
  QMap<QString, QString> defaults;
  defaults.insert(Icons::I_KITTY, ":/icons/main.ico");
  defaults.insert(Icons::I_QUIT, ":/glyphs/quit.png");
  defaults.insert(Icons::I_SETTINGS, ":/glyphs/settings.png");
  defaults.insert(Icons::I_PLUGIN, ":/glyphs/plugin.png");
  defaults.insert(Icons::I_INFO, ":/glyphs/info.png");
  defaults.insert(Icons::I_PROFILE, ":/glyphs/profile.png");
  defaults.insert(Icons::I_USER, ":/glyphs/user.png");
  defaults.insert(Icons::I_FOLDER, ":/glyphs/folder.png");
  defaults.insert(Icons::I_FOLDER_KITTY, ":/glyphs/folder_kitty.png");
  defaults.insert(Icons::I_REFRESH, ":/glyphs/refresh.png");
  defaults.insert(Icons::I_CONSOLE, ":/glyphs/console.png");
  defaults.insert(Icons::I_BULLET, ":/glyphs/bullet.png");
  defaults.insert(Icons::I_CONNECT, ":/glyphs/connect.png");
  defaults.insert(Icons::I_HISTORY, ":/glyphs/history.png");
  defaults.insert(Icons::I_KEY, ":/glyphs/key.png");
  defaults.insert(Icons::I_MESSAGE, ":/glyphs/message.png");
  defaults.insert(Icons::I_PALETTE, ":/glyphs/palette.png");
  defaults.insert(Icons::I_SMILIE, ":/glyphs/smilie.png");
  defaults.insert(Icons::I_TAB, ":/glyphs/tab.png");


  QMapIterator<QString, QString> i(defaults);
  while(i.hasNext()) {
    i.next();
    insert(i.key(), QPixmap(i.value()), false);
  }

}

void IconManager::insert(const QString &id, const QPixmap &pixmap, bool replace)
{
  if(m_icons.contains(id) && !replace) {
    return;
  }

  m_icons.insert(id, pixmap);
}