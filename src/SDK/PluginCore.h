#ifndef SDKPLUGINCORE_H
#define SDKPLUGINCORE_H

#include <QtCore/QVariant>
#include <QtCore/QObject>

class QString;

namespace KittySDK
{
  class PluginCore: public QObject
  {
    Q_OBJECT

    public:
      virtual QVariant setting(const QString &key, const QVariant &defaultValue = QVariant()) = 0;
      virtual void setSetting(const QString &key, const QVariant &value) = 0;
  };
}

#endif // SDKPLUGINCORE_H
