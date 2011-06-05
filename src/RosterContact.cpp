#include "RosterContact.h"

#include "SDK/constants.h"
#include "SDK/Contact.h"
#include "Profile.h"
#include "Core.h"

#include <QtCore/QCryptographicHash>
#include <QtCore/QFile>
#include <QtGui/QTextDocument>

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
  Core *core = Core::inst();

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
      return Core::inst()->avatarPath(m_contact);
    break;

    case Qt::ToolTipRole:
    {
      QString tooltip;

      if(core->setting(Settings::S_ROSTER_TIPS, true).toBool()) {
        tooltip += "<table><tr><td>";
        tooltip += QString("<b>%1</b><br>").arg(Qt::escape(m_contact->display()));
        tooltip += QString("<font size=\"2\">%1</font><br>").arg(Qt::escape(m_contact->uid()));
        tooltip += QString("<font size=\"2\"><b>%1</b>: %2</font><br>").arg(QObject::tr("Account")).arg(Qt::escape(data(RosterItem::AccountRole).toString()));

        if(core->setting(Settings::S_ROSTER_TIPS_EMAIL).toBool()) {
          QString emails = m_contact->data(ContactInfos::I_EMAILS).toString();
          if(!emails.isEmpty()) {
            QStringList list = emails.split(",");
            tooltip += QString("<font size=\"2\"><b>%1</b>: %2</font><br>").arg(QObject::tr("Email")).arg(Qt::escape(list[0]));
          }
        }

        if(core->setting(Settings::S_ROSTER_TIPS_PHONE).toBool()) {
          QString phones = m_contact->data(ContactInfos::I_PHONES).toString();
          if(!phones.isEmpty()) {
            QStringList list = phones.split(",");
            tooltip += QString("<font size=\"2\"><b>%1</b>: %2</font><br>").arg(QObject::tr("Phone")).arg(Qt::escape(list[0]));
          }
        }

        if(core->setting(Settings::S_ROSTER_TIPS_DESCRIPTION).toBool()) {
          QString description = m_contact->description();
          if(!description.isEmpty()) {
            tooltip += QString("<font size=\"2\"><b>%1</b>: %2</font><br>").arg(QObject::tr("Description")).arg(Qt::escape(description).replace('\n', "<br>"));
          }
        }

        tooltip += m_contact->toolTip();

        tooltip += "</td>";

        QString avatar = data(RosterItem::AvatarRole).toString();
        if(QFile::exists(avatar)) {
          tooltip += QString("<td width=\"32\"><img src=\"%1\"></td>").arg(avatar);
        }

        tooltip += "</tr></table>";
      }

      return tooltip;
    }
    break;

    default:
      return RosterItem::data(role);
    break;
  }
}
