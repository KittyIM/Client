#ifndef SDKPLUGIN_H
#define SDKPLUGIN_H

#include "PluginCore.h"

#include <QtCore/QObject>
#include <QtCore/QtPlugin>

class QString;

namespace KittySDK
{
  class PluginInfo
  {
    public:
      PluginInfo(const QString &author = "", const QString &email = "", const QString &www = ""): m_author(author), m_email(email), m_www(www) { }

      QString author() const { return m_author; }
      QString email() const { return m_email; }
      QString www() const { return m_www; }

    private:
      QString m_author;
      QString m_email;
      QString m_www;
  };

  class Plugin: public QObject
  {
    Q_OBJECT

    public:
      Plugin(): QObject(0) { }

      enum { Type = 1 };
      int type() const { return Type; }

      void setCore(PluginCore *core) { m_core = core; }

      PluginInfo *info() const { return m_info; }
      PluginCore *core() const { return m_core; }

    public slots:
      virtual void applySettings() { }

    protected:
      PluginInfo *m_info;
      PluginCore *m_core;
  };
}

#define KITTY_PLUGIN(PLUGINCLASS) \
Q_EXTERN_C Q_DECL_EXPORT QObject *inst() \
{ \
  static QPointer<QObject> m_inst; \
  if(!m_inst) { \
      m_inst = new PLUGINCLASS(); \
  } \
  return m_inst; \
}

#endif // SDKPLUGIN_H
