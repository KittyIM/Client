#ifndef XMLSETTINGS_H
#define XMLSETTINGS_H

#include <QtCore/QSettings>

class QDomElement;

namespace Kitty
{
  class XmlSettings: public QSettings
  {
    Q_OBJECT

    public:
      explicit XmlSettings(const QString &fileName, QObject *parent = 0);

      static void readElement(SettingsMap &map, const QDomElement &root, const QString &name);
      static bool xmlRead(QIODevice &device, SettingsMap &map);
      static bool xmlWrite(QIODevice &device, const SettingsMap &map);

      static QString variantToString(const QVariant &v);
      static QVariant stringToVariant(const QString &s);
      static QStringList splitArgs(const QString &s, int idx);

    public:
      static QSettings::Format xmlFormat;
  };
}

#endif // XMLSETTINGS_H
