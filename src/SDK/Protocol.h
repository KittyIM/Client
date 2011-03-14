#ifndef SDKPROTOCOL_H
#define SDKPROTOCOL_H

#include "Plugin.h"

namespace Kitty
{
  class Protocol: public Plugin
  {
    public:

      enum { Type = 2 };
      int type() const { return Type; }

    private:
  };
}

#endif // SDKPROTOCOL_H
