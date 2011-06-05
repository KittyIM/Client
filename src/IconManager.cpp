#include "IconManager.h"

#include "SDK/constants.h"

#include <QtCore/QMap>
#include <QtCore/QDebug>

#define qDebug() qDebug() << "[IconManager]"
#define qWarning() qWarning() << "[IconManager]"

using namespace KittySDK;

Kitty::IconManager::IconManager(QObject *parent): QObject(parent)
{
  m_updateTimer.setSingleShot(true);
  m_updateTimer.setInterval(100);

  connect(&m_updateTimer, SIGNAL(timeout()), this, SIGNAL(iconsUpdated()));

  loadDefaults();
}

void Kitty::IconManager::loadDefaults()
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
  defaults.insert(Icons::I_SMILEY, ":/glyphs/smiley.png");
  defaults.insert(Icons::I_TAB, ":/glyphs/tab.png");
  defaults.insert(Icons::I_BOLD, ":/glyphs/bold.png");
  defaults.insert(Icons::I_ITALIC, ":/glyphs/italic.png");
  defaults.insert(Icons::I_UNDERLINE, ":/glyphs/underline.png");
  defaults.insert(Icons::I_STRIKETHROUGH, ":/glyphs/strikethrough.png");
  defaults.insert(Icons::I_COLOR, ":/glyphs/color.png");
  defaults.insert(Icons::I_IMAGE, ":/glyphs/image.png");
  defaults.insert(Icons::I_FILE, ":/glyphs/file.png");
  defaults.insert(Icons::I_BLOCK, ":/glyphs/block.png");
  defaults.insert(Icons::I_DELETE, ":/glyphs/delete.png");
  defaults.insert(Icons::I_COPY, ":/glyphs/copy.png");
  defaults.insert(Icons::I_GROUP, ":/glyphs/group.png");
  defaults.insert(Icons::I_SOUND, ":/glyphs/sound.png");
  defaults.insert(Icons::I_ADD, ":/glyphs/add.png");
  defaults.insert(Icons::I_EDIT, ":/glyphs/edit.png");
  defaults.insert(Icons::I_STATUS_ONLINE, ":/glyphs/status_online.png");
  defaults.insert(Icons::I_STATUS_AWAY, ":/glyphs/status_away.png");
  defaults.insert(Icons::I_STATUS_FFC, ":/glyphs/status_ffc.png");
  defaults.insert(Icons::I_STATUS_DND, ":/glyphs/status_dnd.png");
  defaults.insert(Icons::I_STATUS_INVIS, ":/glyphs/status_invisible.png");
  defaults.insert(Icons::I_STATUS_OFFLINE, ":/glyphs/status_offline.png");

  qDebug() << "Loading default icons [" << defaults.count() << "]";

  QMapIterator<QString, QString> i(defaults);
  while(i.hasNext()) {
    i.next();
    insert(i.key(), QPixmap(i.value()), false);
  }
}

void Kitty::IconManager::insert(const QString &id, const QPixmap &pixmap, bool replace)
{
  if(m_icons.contains(id) && !replace) {
    return;
  }

  m_icons.insert(id, pixmap);

  m_updateTimer.start();
}
