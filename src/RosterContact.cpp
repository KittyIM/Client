#include "RosterContact.h"

#include "SDK/Contact.h"

Kitty::RosterContact::RosterContact(KittySDK::Contact *contact, RosterItem *parent): RosterItem(parent), m_contact(contact)
{
  setData(Kitty::RosterItem::Contact, Kitty::RosterItem::TypeRole);
}

QVariant Kitty::RosterContact::data(int role) const
{
  switch(role) {
    case Qt::DisplayRole:
      return m_contact->display();
    break;

    case RosterItem::AccountRole:
      return QString("%1 (%2)").arg(m_contact->account()->uid()).arg(m_contact->account()->protocol()->protoInfo()->protoName());
    break;

    case RosterItem::UidRole:
      return m_contact->uid();
    break;

    default:
      return RosterItem::data(role);
    break;
  }
}
