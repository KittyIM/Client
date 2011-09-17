#include "JsonSettings.h"

#include "3rdparty/json/json.h"

#include <QtGui/QMessageBox>
#include <QtCore/QStringList>
#include <QtCore/QDebug>
#include <QtCore/QPoint>
#include <QtCore/QRect>
#include <QtCore/QSize>

#define qDebug() qDebug() << "[JsonSettings]"
#define qWarning() qWarning() << "[JsonSettings]"

QSettings::Format Kitty::JsonSettings::jsonFormat = QSettings::registerFormat("json", jsonRead, jsonWrite);

Kitty::JsonSettings::JsonSettings(const QString &fileName, QObject *parent): QSettings(fileName, JsonSettings::jsonFormat, parent)
{
  qDebug() << "Loading" << fileName;
}

void Kitty::JsonSettings::readMap(SettingsMap &map, const QVariant &root, const QString &name)
{
  if(root.type() == QVariant::Map) {
    QVariantMap vmap = root.toMap();
    QMapIterator<QString, QVariant> it(vmap);
    while(it.hasNext()) {
      it.next();

      QString newName = name;
      if(!newName.isEmpty()) {
        newName.append(".");
      }

      newName.append(it.key());

      readMap(map, it.value(), newName);
    }
  } else {
    map.insert(name, stringToVariant(root.toString()));
  }
}

void Kitty::JsonSettings::writeMap(QVariant &root, const QString &key, const QVariant &value)
{
  int pos = key.indexOf('.');

  if(pos < 0) {
    QVariantMap map = root.toMap();
    if((value.type() == QVariant::Map) || (value.type() == QVariant::List)) {
      map.insert(key, value);
    } else {
      map.insert(key, variantToString(value));
    }

    root = map;

    return;
  }

  QString groupName = key.left(pos);

  QVariantMap map = root.toMap();
  QVariant item = map.value(groupName);
  writeMap(item, key.mid(pos + 1), value);
  map.insert(groupName, item);

  root = map;
}

bool Kitty::JsonSettings::jsonRead(QIODevice &device, SettingsMap &map)
{
  QVariant vmap = Json::parse(qUncompress(device.readAll()));
  readMap(map, vmap, "");

  qDebug() << "Read" << map.count() << "positions";

  return true;
}

bool Kitty::JsonSettings::jsonWrite(QIODevice &device, const SettingsMap &map)
{
  qDebug() << "Writing";

  QVariant vmap;

  QMapIterator<QString, QVariant> i(map);
  while(i.hasNext()) {
    i.next();

    writeMap(vmap, i.key(), i.value());
  }

  QString json = Json::stringify(vmap);

  device.write(qCompress(json.toAscii()));

  qDebug() << "Wrote" << map.count() << "positions," << json.count() << "bytes";

  return true;
}

// This function is taken from Qt's qsettings.cpp (except QByteArrays are saved in Base64)
QString Kitty::JsonSettings::variantToString(const QVariant &v)
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
QVariant Kitty::JsonSettings::stringToVariant(const QString &s)
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
        QStringList args = JsonSettings::splitArgs(s, 5);
        if(args.size() == 4) {
          return QVariant(QRect(args[0].toInt(), args[1].toInt(), args[2].toInt(), args[3].toInt()));
        }
      } else if(s.startsWith(QLatin1String("@Size("))) {
        QStringList args = JsonSettings::splitArgs(s, 5);
        if(args.size() == 2) {
          return QVariant(QSize(args[0].toInt(), args[1].toInt()));
        }
      } else if(s.startsWith(QLatin1String("@Point("))) {
        QStringList args = JsonSettings::splitArgs(s, 6);
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
QStringList Kitty::JsonSettings::splitArgs(const QString &s, int idx)
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
