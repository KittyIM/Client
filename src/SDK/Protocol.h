#ifndef SDKPROTOCOL_H
#define SDKPROTOCOL_H

#include "Plugin.h"

namespace KittySDK
{
  class ProtocolInfo: public PluginInfo
  {
    public:
      ProtocolInfo(const QString &name = "", const QString &version = "", const QString &author = "", const QString &email = "", const QString &www = "", const QString &protoName = "", const QString &protoIcon = ""):
        PluginInfo(name, version, author, email, www),
        m_protoName(protoName),
        m_protoIcon(protoIcon)
      { }

      QString protoName() const { return m_protoName; }
      void setProtoName(const QString &protoName) { m_protoName = protoName; }

      QString protoIcon() const { return m_protoIcon; }
      void setProtoIcon(const QString &protoIcon) { m_protoIcon = protoIcon; }

    private:
      QString m_protoName;
      QString m_protoIcon;
  };

  class Protocol: public Plugin
  {
    Q_OBJECT

    public:
      enum Status
      {
        Online = 1,
        Away,
        FFC,
        DND,
        Invisible,
        Offline
      };

    public:
      Protocol(PluginCore *core): Plugin(core) { }

      ProtocolInfo *protoInfo() const { return static_cast<ProtocolInfo*>(m_info); }

      virtual Account *newAccount(const QString &uid) = 0;
      virtual QWidget *editWindow(Account *account = 0) = 0;

      enum { Type = 2 };
      int type() const { return Type; }
  };
}

#endif // SDKPROTOCOL_H
