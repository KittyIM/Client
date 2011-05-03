#include "RosterContact.h"

#include "SDK/Contact.h"
#include "Profile.h"
#include "Core.h"

#define qDebug() qDebug() << "[RosterContact]"
#define qWarning() qWarning() << "[RosterContact]"

using namespace Kitty;
using namespace KittySDK;

Kitty::RosterContact::RosterContact(KittySDK::Contact *contact, RosterItem *parent): RosterItem(parent), m_contact(contact)
{
  setData(RosterItem::Contact, RosterItem::TypeRole);
}

QVariant Kitty::RosterContact::data(int role) const
{
  switch(role) {
    case Qt::DisplayRole:
      return m_contact->display();
    break;

    case Qt::DecorationRole:
      return Core::inst()->icon(m_contact->account()->protocol()->statusIcon(m_contact->status()));
    break;

    case RosterItem::AccountRole:
      return QString("%1 (%2)").arg(m_contact->account()->uid()).arg(m_contact->account()->protocol()->protoInfo()->protoName());
    break;

    case RosterItem::UidRole:
      return m_contact->uid();
    break;

    case RosterItem::StatusRole:
      return m_contact->status();
    break;

    case RosterItem::DescriptionRole:
      return m_contact->description();
    break;

    case RosterItem::AvatarRole:
      return Core::inst()->profilesDir() + Core::inst()->profile()->name() + "/avatars/avatar_" + m_contact->uid() + ".png";
    break;

    case Qt::ToolTipRole:
      return QString("<b>%1</b><br>Account: %2<br>Uid: %3<br>Group: %4").arg(data(Qt::DisplayRole).toString()).arg(data(RosterItem::AccountRole).toString()).arg(data(RosterItem::UidRole).toString()).arg("");
    break;

    default:
      return RosterItem::data(role);
    break;
  }
}
