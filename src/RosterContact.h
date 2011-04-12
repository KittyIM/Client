#ifndef ROSTERCONTACT_H
#define ROSTERCONTACT_H

#include "RosterItem.h"

namespace KittySDK
{
  class Contact;
}

namespace Kitty
{
  class RosterContact: public RosterItem
  {
    public:
      explicit RosterContact(KittySDK::Contact *contact, RosterItem *parent = 0);

      KittySDK::Contact *contact() const { return m_contact; }

      QVariant data(int role = Qt::UserRole + 1) const;


    private:
      KittySDK::Contact *m_contact;
  };
}

#endif // KITTY_ROSTERCONTACT_H
