#include "EmoticonPack.h"

#include "EmoticonManager.h"

#include <QtCore/QTextCodec>
#include <QtCore/QDebug>
#include <QtCore/QDir>
#include <QtGui/QApplication>
#include <QtXml/QDomDocument>
#include <QtXml/QDomElement>

#define qDebug() qDebug() << "[EmoticonPack]"
#define qWarning() qWarning() << "[EmoticonPack]"

using namespace Kitty;

Kitty::EmoticonPack::EmoticonPack(const QString &name, QObject *parent): QObject(parent), m_name(name)
{
  QDir dir(qApp->applicationDirPath() + "/emoticons/" + name);
  if(dir.exists()) {
    if(dir.exists("emots.txt")) { //old gadu-gadu format
      QFile file(dir.filePath("emots.txt"));
      if(file.open(QFile::ReadOnly)) {
        QTextStream str(&file);
        str.setCodec(QTextCodec::codecForName("Windows-1250"));
        while(!str.atEnd()) {
          QString line = str.readLine();
          if(!line.isEmpty()) {
            if(line[0] == '*') {
              line.remove(0, 1);
            }

            if(line[0] == '(') {
              QRegExp multi("\\((\".*\")\\)");
              multi.setMinimal(true);
              multi.indexIn(line, 0);

              QString cap = multi.cap();
              cap.remove(0, 1);
              cap.chop(1);

              int startPos = multi.pos() + multi.matchedLength() + 2;
              QString fileName = line.mid(startPos, line.indexOf("\"", startPos) - startPos);

              Emoticon *emot = new Emoticon(qApp->applicationDirPath() + "/emoticons/" + name + "/" + fileName);

              QStringList texts = cap.split(",");
              foreach(QString text, texts) {
                text.remove(0, 1);
                text.chop(1);

                emot->addText(text);
              }

              m_emots.append(emot);
            } else {
              QStringList texts = line.split(",");
              QString text = texts[0];
              text.remove(0, 1);
              text.chop(1);

              QString fileName = texts[1];
              fileName.remove(0, 1);
              fileName.chop(1);

              m_emots.append(new Emoticon(qApp->applicationDirPath() + "/emoticons/" + name + "/" + fileName, text));
            }
          }
        }

        file.close();
      } else {
        qWarning() << "emots.txt open failed for" << name;
      }
    } else if(dir.exists("emots.xml")) { //new gadu-gadu format
      QFile file(dir.filePath("emots.xml"));
      if(file.open(QFile::ReadOnly)) {
        QDomDocument doc;
        doc.setContent(&file);

        QDomElement root = doc.documentElement();
        if(root.nodeName() == "Emots") {
          QDomNodeList emots = root.elementsByTagName("Emot");
          for(int i = 0; i < emots.count(); i++) {
            QDomElement emot = emots.at(i).toElement();

            QDomElement animation = emot.namedItem("Animation").toElement();
            QString fileName = qApp->applicationDirPath() + "/emoticons/" + name + "/" + animation.firstChild().nodeValue();

            Emoticon *emo = new Emoticon(fileName);

            QDomNodeList codes = emot.elementsByTagName("Code");
            for(int j = 0; j < codes.count(); j++) {
              emo->addText(codes.at(j).firstChild().nodeValue());
            }

            m_emots.append(emo);
          }
        } else {
          qWarning() << "Wrong xml root" << name;
        }

        file.close();
      }
    }
  } else {
    qWarning() << "Dir not found" << name;
  }
}

Kitty::EmoticonPack::~EmoticonPack()
{
  qDeleteAll(m_emots);
}
