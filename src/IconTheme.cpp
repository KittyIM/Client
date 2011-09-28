#include "IconTheme.h"

#include "SDK/constants.h"
#include "Core.h"

#include <QtCore/QDebug>
#include <QtCore/QFile>
#include <QtCore/QDir>
#include <QtGui/QApplication>
#include <QtXml/QDomDocument>
#include <QtXml/QDomElement>

#define qDebug() qDebug() << "[IconTheme]"
#define qWarning() qWarning() << "[IconTheme]"

using namespace KittySDK;

Kitty::IconTheme::IconTheme(const QString &name)
{
  loadDefaults();

  if(name.isEmpty()) {
    return;
  }

  if(QDir(qApp->applicationDirPath() + "/themes/icon/" + name).exists()) {
    QFile file(qApp->applicationDirPath() + "/themes/icon/" + name + "/theme.xml");
    if(file.open(QIODevice::ReadOnly)) {
      QDomDocument doc;
      doc.setContent(&file);

      QDomElement root = doc.documentElement();
      QDomNodeList children = root.childNodes();
      for(int i = 0; i < children.count(); i++) {
        QDomNode child = children.at(i);

        if(child.nodeName() == "info") {
          QDomNodeList infos = child.childNodes();
          for(int j = 0; j < infos.count(); j++) {
            QDomNode info = infos.at(j);

            if(info.nodeName() == "name") {
              m_name = info.firstChild().toText().nodeValue();
            } else if(info.nodeName() == "author") {
              m_author = info.firstChild().toText().nodeValue();
            } else if(info.nodeName() == "email") {
              m_email = info.firstChild().toText().nodeValue();
            }
          }
        } else if(child.nodeName() == "icons") {
          QDomNodeList icons = child.toElement().elementsByTagName("icon");
          for(int j = 0; j < icons.count(); j++) {
            QDomElement icon = icons.at(j).toElement();

            m_icons.insert(icon.attribute("id"), qApp->applicationDirPath() + "/themes/icon/" + name + "/" + icon.attribute("filename"));
          }
        }
      }

      file.close();
    } else {
      qDebug() << "Could not open icon theme file" << name;
    }
  } else {
    qDebug() << "Icon theme directory does not exist" << name;
  }
}

void Kitty::IconTheme::loadDefaults()
{
  m_icons.insert(Icons::I_KITTY, ":/icons/main.ico");
  m_icons.insert(Icons::I_QUIT, ":/glyphs/quit.png");
  m_icons.insert(Icons::I_SETTINGS, ":/glyphs/settings.png");
  m_icons.insert(Icons::I_PLUGIN, ":/glyphs/plugin.png");
  m_icons.insert(Icons::I_INFO, ":/glyphs/info.png");
  m_icons.insert(Icons::I_PROFILE, ":/glyphs/profile.png");
  m_icons.insert(Icons::I_USER, ":/glyphs/user.png");
  m_icons.insert(Icons::I_FOLDER, ":/glyphs/folder.png");
  m_icons.insert(Icons::I_FOLDER_KITTY, ":/glyphs/folder_kitty.png");
  m_icons.insert(Icons::I_REFRESH, ":/glyphs/refresh.png");
  m_icons.insert(Icons::I_CONSOLE, ":/glyphs/console.png");
  m_icons.insert(Icons::I_BULLET, ":/glyphs/bullet.png");
  m_icons.insert(Icons::I_CONNECT, ":/glyphs/connect.png");
  m_icons.insert(Icons::I_HISTORY, ":/glyphs/history.png");
  m_icons.insert(Icons::I_KEY, ":/glyphs/key.png");
  m_icons.insert(Icons::I_MESSAGE, ":/glyphs/message.png");
  m_icons.insert(Icons::I_PALETTE, ":/glyphs/palette.png");
  m_icons.insert(Icons::I_SMILEY, ":/glyphs/smiley.png");
  m_icons.insert(Icons::I_TAB, ":/glyphs/tab.png");
  m_icons.insert(Icons::I_BOLD, ":/glyphs/bold.png");
  m_icons.insert(Icons::I_ITALIC, ":/glyphs/italic.png");
  m_icons.insert(Icons::I_UNDERLINE, ":/glyphs/underline.png");
  m_icons.insert(Icons::I_STRIKETHROUGH, ":/glyphs/strikethrough.png");
  m_icons.insert(Icons::I_COLOR, ":/glyphs/color.png");
  m_icons.insert(Icons::I_IMAGE, ":/glyphs/image.png");
  m_icons.insert(Icons::I_FILE, ":/glyphs/file.png");
  m_icons.insert(Icons::I_BLOCK, ":/glyphs/block.png");
  m_icons.insert(Icons::I_DELETE, ":/glyphs/delete.png");
  m_icons.insert(Icons::I_COPY, ":/glyphs/copy.png");
  m_icons.insert(Icons::I_GROUP, ":/glyphs/group.png");
  m_icons.insert(Icons::I_SOUND, ":/glyphs/sound.png");
  m_icons.insert(Icons::I_PRINTER, ":/glyphs/printer.png");
  m_icons.insert(Icons::I_FILTER, ":/glyphs/filter.png");
  m_icons.insert(Icons::I_ADD, ":/glyphs/add.png");
  m_icons.insert(Icons::I_EDIT, ":/glyphs/edit.png");
  m_icons.insert(Icons::I_STATUS_ONLINE, ":/glyphs/status_online.png");
  m_icons.insert(Icons::I_STATUS_AWAY, ":/glyphs/status_away.png");
  m_icons.insert(Icons::I_STATUS_FFC, ":/glyphs/status_ffc.png");
  m_icons.insert(Icons::I_STATUS_DND, ":/glyphs/status_dnd.png");
  m_icons.insert(Icons::I_STATUS_INVIS, ":/glyphs/status_invisible.png");
  m_icons.insert(Icons::I_STATUS_OFFLINE, ":/glyphs/status_offline.png");
}
