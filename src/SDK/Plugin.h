#ifndef SDKPLUGIN_H
#define SDKPLUGIN_H

#include "PluginCore.h"

#include <QtCore/QObject>
#include <QtCore/QtPlugin>

class QString;
class QWidget;

namespace KittySDK
{
  class Account;

  class PluginInfo
  {
    public:
      PluginInfo(const QString &name = "", const QString &version = "", const QString &author = "", const QString &email = "", const QString &www = ""):
        m_name(name),
        m_version(version),
        m_author(author),
        m_email(email),
        m_www(www)
      { }

      QString name() const { return m_name; }
      void setName(const QString &name) { m_name = name; }

      QString version() const { return m_version; }
      void setVersion(const QString &version) { m_version = version; }

      QString author() const { return m_author; }
      void setAuthor(const QString &author) { m_author = author; }

      QString email() const { return m_email; }
      void setEmail(const QString &email) { m_email = email; }

      QString www() const { return m_www; }
      void setWWW(const QString &www) { m_www = www; }

    private:
      QString m_name;
      QString m_version;
      QString m_author;
      QString m_email;
      QString m_www;
  };

  class Plugin: public QObject
  {
    Q_OBJECT

    public:
      Plugin(PluginCore *core): QObject(0), m_core(core) { }

      enum { Type = 1 };
      int type() const { return Type; }

      PluginInfo *info() const { return m_info; }
      PluginCore *core() const { return m_core; }

    public slots:
      virtual void applySettings() { }
      virtual void updateIcons() { }

    protected:
      PluginInfo *m_info;
      PluginCore *m_core;
  };
}

#define KITTY_PLUGIN(PLUGINCLASS) \
Q_EXTERN_C Q_DECL_EXPORT QObject *inst(PluginCore *core) \
{ \
  static QPointer<QObject> m_inst; \
  if(!m_inst) { \
      m_inst = new PLUGINCLASS(core); \
  } \
  return m_inst; \
}

#endif // SDKPLUGIN_H
