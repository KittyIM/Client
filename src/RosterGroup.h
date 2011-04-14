#ifndef ROSTERGROUP_H
#define ROSTERGROUP_H

#include "RosterItem.h"

namespace Kitty
{
  class RosterGroup: public RosterItem
  {
    public:
      explicit RosterGroup(RosterItem *parent = 0);
  };
}

#endif // ROSTERGROUP_H
