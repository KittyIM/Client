#include "KittyIconMngr.h"
#include "SDK/constants.h"

using namespace KittySDK;

KittyIconMngr::KittyIconMngr(QObject *parent): QObject(parent)
{
  loadDefaults();
}

void KittyIconMngr::loadDefaults()
{
  QMap<int, QString> defaults;
  defaults.insert(Icons::KITTY, ":/icons/main.ico");
  defaults.insert(Icons::QUIT, ":/glyphs/quit.png");
  defaults.insert(Icons::SETTINGS, ":/glyphs/settings.png");
  defaults.insert(Icons::PLUGIN, ":/glyphs/plugin.png");
  defaults.insert(Icons::INFO, ":/glyphs/info.png");
  defaults.insert(Icons::PROFILE, ":/glyphs/profile.png");
  defaults.insert(Icons::USER, ":/glyphs/user.png");
  defaults.insert(Icons::FOLDER, ":/glyphs/folder.png");
  defaults.insert(Icons::FOLDER_KITTY, ":/glyphs/folder_kitty.png");
  defaults.insert(Icons::REFRESH, ":/glyphs/refresh.png");
  defaults.insert(Icons::CONSOLE, ":/glyphs/console.png");
  //defaults.insert(Icons::, ":/glyphs/.png");

  QMapIterator<int, QString> i(defaults);
  while(i.hasNext()) {
    i.next();
    addIcon(i.key(), QPixmap(i.value()), false);
  }

}

void KittyIconMngr::addIcon(const int &id, const QPixmap &pixmap, bool replace)
{
  if(m_icons.contains(id) && !replace) {
    return;
  }

  m_icons.insert(id, pixmap);
}

QPixmap KittyIconMngr::getIcon(const int &id)
{
  return m_icons.value(id);
}
