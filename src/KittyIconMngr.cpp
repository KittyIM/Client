#include "KittyIconMngr.h"
#include "SDK/constants.h"

using namespace KittySDK;

KittyIconMngr::KittyIconMngr(QObject *parent): QObject(parent)
{
  loadDefaults();
}

void KittyIconMngr::loadDefaults()
{
  addIcon(Icons::QUIT, QPixmap(":/glyphs/quit.png"), false);
  addIcon(Icons::SETTINGS, QPixmap(":/glyphs/settings.png"), false);
  addIcon(Icons::PLUGIN, QPixmap(":/glyphs/plugin.png"), false);

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
