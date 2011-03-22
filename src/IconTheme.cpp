#include "IconTheme.h"

#include "Core.h"

#include <QtCore/QDebug>
#include <QtCore/QFile>
#include <QtCore/QDir>
#include <QtGui/QApplication>
#include <QtXml/QDomDocument>
#include <QtXml/QDomElement>

Kitty::IconTheme::IconTheme(const QString &name)
{
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
