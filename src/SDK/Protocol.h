#ifndef SDKPROTOCOL_H
#define SDKPROTOCOL_H

#include "Plugin.h"

namespace KittySDK
{
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

      virtual Account *newAccount(const QString &uid) = 0;
      virtual QWidget *editWindow(Account *account) = 0;

      enum { Type = 2 };
      int type() const { return Type; }
  };
}

#endif // SDKPROTOCOL_H
