#include "RosterTheme.h"

#include <QtCore/QDebug>
#include <QtCore/QFile>
#include <QtCore/QDir>
#include <QtGui/QApplication>
#include <QtXml/QDomDocument>
#include <QtXml/QDomElement>

Kitty::RosterTheme::RosterTheme(const QString &name)
{
  QString fileName;
  if(!name.isEmpty() && QDir(qApp->applicationDirPath() + "/themes/roster/" + name).exists()) {
    fileName = qApp->applicationDirPath() + "/themes/roster/" + name + "/theme.xml";
    qDebug() << "Loading" << fileName;
  } else {
    fileName = ":/roster/theme.xml";
    qDebug() << "Loading default roster theme.";
  }

  QFile file(fileName);
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
      } else if(child.nodeName() == "brushes") {
        QDomNodeList infos = child.childNodes();
        for(int j = 0; j < infos.count(); j++) {
          QDomNode info = infos.at(j);
          QByteArray arr = QByteArray::fromBase64(info.firstChild().toText().nodeValue().toAscii());
          QDataStream str(&arr, QIODevice::ReadOnly);

          if(info.nodeName() == "group") {
            str >> m_groupBackground;
          } else if(info.nodeName() == "selectedGroup") {
            str >> m_selectedGroupBackground;
          } else if(info.nodeName() == "contact") {
            str >> m_contactBackground;
          } else if(info.nodeName() == "selectedContact") {
            str >> m_selectedContactBackground;
          }
        }
      } else if(child.nodeName() == "pens") {
        QDomNodeList infos = child.childNodes();
        for(int j = 0; j < infos.count(); j++) {
          QDomNode info = infos.at(j);
          QByteArray arr = QByteArray::fromBase64(info.firstChild().toText().nodeValue().toAscii());
          QDataStream str(&arr, QIODevice::ReadOnly);

          if(info.nodeName() == "group") {
            str >> m_groupForeground;
          } else if(info.nodeName() == "selectedGroup") {
            str >> m_selectedGroupForeground;
          } else if(info.nodeName() == "contact") {
            str >> m_contactForeground;
          } else if(info.nodeName() == "selectedContact") {
            str >> m_selectedContactForeground;
          }
        }
      } else if(child.nodeName() == "colors") {
        QDomNodeList infos = child.childNodes();
        for(int j = 0; j < infos.count(); j++) {
          QDomNode info = infos.at(j);
          QByteArray arr = QByteArray::fromBase64(info.firstChild().toText().nodeValue().toAscii());
          QDataStream str(&arr, QIODevice::ReadOnly);

          if(info.nodeName() == "groupName") {
            str >> m_groupNameColor;
          } else if(info.nodeName() == "contactName") {
            str >> m_contactNameColor;
          } else if(info.nodeName() == "contactDescription") {
            str >> m_contactDescriptionColor;
          }
        }
      } else if(child.nodeName() == "fonts") {
        QDomNodeList infos = child.childNodes();
        for(int j = 0; j < infos.count(); j++) {
          QDomNode info = infos.at(j);
          QByteArray arr = QByteArray::fromBase64(info.firstChild().toText().nodeValue().toAscii());
          QDataStream str(&arr, QIODevice::ReadOnly);

          if(info.nodeName() == "group") {
            str >> m_groupFont;
          } else if(info.nodeName() == "contact") {
            str >> m_contactFont;
          } else if(info.nodeName() == "description") {
            str >> m_descriptionFont;
          }
        }
      }
    }

    file.close();
  }
}

