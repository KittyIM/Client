#ifndef SDKPLUGINCORE_H
#define SDKPLUGINCORE_H

#include <QtCore/QVariant>
#include <QtCore/QObject>

class QString;

namespace KittySDK
{
  class SettingPage;

  class PluginCore: public QObject
  {
    Q_OBJECT

    public:
      virtual QVariant setting(const QString &key, const QVariant &defaultValue = QVariant()) = 0;
      virtual void setSetting(const QString &key, const QVariant &value) = 0;

      virtual void addSettingPage(SettingPage *page, const QString &parent) = 0;

      virtual QPixmap icon(const QString &id) = 0;
      virtual void addIcon(const QString &id, const QPixmap &pixmap, bool replace = true) = 0;
  };
}

#endif // SDKPLUGINCORE_H
