#include "XmlSettings.h"

#include <QtCore/QStringList>
#include <QtCore/QDebug>
#include <QtCore/QPoint>
#include <QtCore/QRect>
#include <QtCore/QSize>
#include <QtXml/QDomDocument>
#include <QtXml/QDomElement>

QSettings::Format Kitty::XmlSettings::xmlFormat = QSettings::registerFormat("xml", XmlSettings::xmlRead, XmlSettings::xmlWrite);

Kitty::XmlSettings::XmlSettings(const QString &fileName, QObject *parent): QSettings(fileName, XmlSettings::xmlFormat, parent)
{

}

void Kitty::XmlSettings::readElement(SettingsMap &map, const QDomElement &root, const QString &name)
{
  QDomNodeList children = root.childNodes();
  for(int i = 0; i < children.count(); i++) {
    QDomNode child = children.at(i);

    if(child.isCDATASection()) {
      map.insert(name, XmlSettings::stringToVariant(child.nodeValue()));
    } else if(child.isElement()) {
      QString newName = name;
      if(!newName.isEmpty()) {
        newName.append(".");
      }
      newName.append(child.nodeName());

      readElement(map, child.toElement(), newName);
    }
  }
}

bool Kitty::XmlSettings::xmlRead(QIODevice &device, SettingsMap &map)
{
  QDomDocument doc;
  doc.setContent(&device);

  QDomElement root = doc.documentElement();
  if(root.nodeName() != "settings") {
    return false;
  }

  readElement(map, root, "");

  return true;
}

bool Kitty::XmlSettings::xmlWrite(QIODevice &device, const SettingsMap &map)
{
  QDomDocument doc;

  QDomElement root = doc.createElement("settings");

  QMapIterator<QString, QVariant> i(map);
  while(i.hasNext()) {
    i.next();

    QString key = i.key();
    QStringList parts = key.replace("/", ".").split(".");
    QDomElement elem = root;
    foreach(QString part, parts) {
      if(elem.firstChildElement(part).isNull()) {
        elem.appendChild(doc.createElement(part));
      }

      elem = elem.firstChildElement(part);
    }

    elem.appendChild(doc.createCDATASection(XmlSettings::variantToString(i.value())));
  }

  doc.appendChild(root);

  QTextStream str(&device);
  str.setCodec("UTF-8");
  str << doc.toString(2);

  return true;
}

// This function is taken from Qt's qsettings.cpp (except QByteArrays are saved in Base64)
QString Kitty::XmlSettings::variantToString(const QVariant &v)
{
  QString result;

  switch(v.type()) {
    case QVariant::Invalid:
        result = QLatin1String("@Invalid()");
    break;

    case QVariant::ByteArray:
    {
      QByteArray a = v.toByteArray().toBase64();
      result = QLatin1String("@ByteArray(");
      result += QString::fromLatin1(a.constData(), a.size());
      result += QLatin1Char(')');
      break;
    }

    case QVariant::String:
    case QVariant::LongLong:
    case QVariant::ULongLong:
    case QVariant::Int:
    case QVariant::UInt:
    case QVariant::Bool:
    case QVariant::Double:
    case QVariant::KeySequence:
    {
      result = v.toString();
      if(result.startsWith(QLatin1Char('@'))) {
        result.prepend(QLatin1Char('@'));
      }
      break;
    }

#ifndef QT_NO_GEOM_VARIANT
    case QVariant::Rect:
    {
      QRect r = qvariant_cast<QRect>(v);
      result += QLatin1String("@Rect(");
      result += QString::number(r.x());
      result += QLatin1Char(' ');
      result += QString::number(r.y());
      result += QLatin1Char(' ');
      result += QString::number(r.width());
      result += QLatin1Char(' ');
      result += QString::number(r.height());
      result += QLatin1Char(')');
      break;
    }
    case QVariant::Size:
    {
      QSize s = qvariant_cast<QSize>(v);
      result += QLatin1String("@Size(");
      result += QString::number(s.width());
      result += QLatin1Char(' ');
      result += QString::number(s.height());
      result += QLatin1Char(')');
      break;
    }
    case QVariant::Point:
    {
      QPoint p = qvariant_cast<QPoint>(v);
      result += QLatin1String("@Point(");
      result += QString::number(p.x());
      result += QLatin1Char(' ');
      result += QString::number(p.y());
      result += QLatin1Char(')');
      break;
    }
#endif

    default:
    {
#ifndef QT_NO_DATASTREAM
      QByteArray a;
      {
        QDataStream s(&a, QIODevice::WriteOnly);
        s.setVersion(QDataStream::Qt_4_0);
        s << v;
      }

      result = QLatin1String("@Variant(");
      result += QString::fromLatin1(a.constData(), a.size());
      result += QLatin1Char(')');
#endif
      break;
    }
  }

  return result;
}

// This function is taken from Qt's qsettings.cpp (except QByteArrays are saved in Base64)
QVariant Kitty::XmlSettings::stringToVariant(const QString &s)
{
  if(s.startsWith(QLatin1Char('@'))) {
    if(s.endsWith(QLatin1Char(')'))) {
      if(s.startsWith(QLatin1String("@ByteArray("))) {
        return QVariant(QByteArray::fromBase64(s.toLatin1().mid(11, s.size() - 12)));
      } else if (s.startsWith(QLatin1String("@Variant("))) {
#ifndef QT_NO_DATASTREAM
        QByteArray a(s.toLatin1().mid(9));
        QDataStream stream(&a, QIODevice::ReadOnly);
        stream.setVersion(QDataStream::Qt_4_0);
        QVariant result;
        stream >> result;
        return result;
#endif

#ifndef QT_NO_GEOM_VARIANT
      } else if(s.startsWith(QLatin1String("@Rect("))) {
        QStringList args = XmlSettings::splitArgs(s, 5);
        if(args.size() == 4) {
          return QVariant(QRect(args[0].toInt(), args[1].toInt(), args[2].toInt(), args[3].toInt()));
        }
      } else if(s.startsWith(QLatin1String("@Size("))) {
        QStringList args = XmlSettings::splitArgs(s, 5);
        if(args.size() == 2) {
          return QVariant(QSize(args[0].toInt(), args[1].toInt()));
        }
      } else if(s.startsWith(QLatin1String("@Point("))) {
        QStringList args = XmlSettings::splitArgs(s, 6);
        if(args.size() == 2) {
          return QVariant(QPoint(args[0].toInt(), args[1].toInt()));
        }
#endif
      } else if(s == QLatin1String("@Invalid()")) {
        return QVariant();
      }
    }

    if(s.startsWith(QLatin1String("@@"))) {
      return QVariant(s.mid(1));
    }
  }

  return QVariant(s);
}

// This function is taken from Qt's qsettings.cpp
QStringList Kitty::XmlSettings::splitArgs(const QString &s, int idx)
{
  int l = s.length();
  QStringList result;
  QString item;

  for (++idx; idx < l; ++idx) {
    QChar c = s.at(idx);

    if(c == QLatin1Char(')')) {
      result.append(item);
    } else if(c == QLatin1Char(' ')) {
      result.append(item);
      item.clear();
    } else {
      item.append(c);
    }
  }

  return result;
}
