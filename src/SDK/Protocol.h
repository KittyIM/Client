#ifndef SDKPROTOCOL_H
#define SDKPROTOCOL_H

#include "Plugin.h"

namespace KittySDK
{
  class Protocol: public Plugin
  {
    public:
      Protocol(PluginCore *core): Plugin(0) { }

      enum { Type = 2 };
      int type() const { return Type; }

    private:
  };
}

#endif // SDKPROTOCOL_H
