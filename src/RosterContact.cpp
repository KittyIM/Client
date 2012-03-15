#include "RosterContact.h"

#include "Profile.h"
#include "Core.h"

#include <SDKConstants.h>
#include <IContact.h>

#include <QtCore/QCryptographicHash>
#include <QtCore/QFile>
#include <QtGui/QTextDocument>

#define qDebug() qDebug() << "[RosterContact]"
#define qWarning() qWarning() << "[RosterContact]"

namespace Kitty
{

RosterContact::RosterContact(KittySDK::IContact *contact, Core *core, RosterItem *parent):
	RosterItem(parent),
	m_contact(contact),
	m_core(core)
{
	setData(RosterItem::Contact, RosterItem::TypeRole);
	setData(false, BlinkRole);
}

QVariant RosterContact::data(int role) const
{
	switch(role) {
		case Qt::DisplayRole:
			return m_contact->display();
		break;

		case Qt::DecorationRole:
		{
			if(data(BlinkRole).toBool()) {
				return m_core->icon(KittySDK::Icons::I_MESSAGE);
			} else {
				if(KittySDK::IAccount *acc = m_contact->account()) {
					if(KittySDK::IProtocol *proto = acc->protocol()) {
						return m_core->icon(proto->statusIcon(m_contact->status()));
					}
				}

				return QVariant();
			}
		}
		break;

		case RosterItem::AccountRole:
		{
			if(KittySDK::IAccount *acc = m_contact->account()) {
				return acc->uid();
			}

			return QVariant();
		}
		break;

		case RosterItem::ProtocolRole:
		{
			if(KittySDK::IAccount *acc = m_contact->account()) {
				if(KittySDK::IProtocol *proto = acc->protocol()) {
					if(KittySDK::IProtocolInfo *info = proto->protoInfo()) {
						return info->protoName();
					}
				}
			}

			return QVariant();
		}
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
			return m_core->avatarPath(m_contact);
		break;

		case RosterItem::TemporaryRole:
			return m_contact->data(KittySDK::ContactInfos::I_TEMPORARY);
		break;

		case Qt::ToolTipRole:
		{
			QString tooltip;

			if(m_core->setting(KittySDK::Settings::S_ROSTER_TIPS, true).toBool()) {
				tooltip += "<table><tr><td>";
				tooltip += QString("<b>%1</b><br>").arg(Qt::escape(m_contact->display()));
				tooltip += QString("<font size=\"2\">%1</font><br>").arg(Qt::escape(m_contact->uid()));
				tooltip += QString("<font size=\"2\"><b>%1</b>: %2</font><br>").arg(QObject::tr("Account")).arg(Qt::escape(QString("%1 (%2)").arg(data(RosterItem::AccountRole).toString()).arg(data(RosterItem::ProtocolRole).toString())));

				if(m_core->setting(KittySDK::Settings::S_ROSTER_TIPS_EMAIL).toBool()) {
					QString emails = m_contact->data(KittySDK::ContactInfos::I_EMAILS).toString();
					if(!emails.isEmpty()) {
						QStringList list = emails.split(",");
						tooltip += QString("<font size=\"2\"><b>%1</b>: %2</font><br>").arg(QObject::tr("Email")).arg(Qt::escape(list[0]));
					}
				}

				if(m_core->setting(KittySDK::Settings::S_ROSTER_TIPS_PHONE).toBool()) {
					QString phones = m_contact->data(KittySDK::ContactInfos::I_PHONES).toString();
					if(!phones.isEmpty()) {
						QStringList list = phones.split(",");
						tooltip += QString("<font size=\"2\"><b>%1</b>: %2</font><br>").arg(QObject::tr("Phone")).arg(Qt::escape(list[0]));
					}
				}

				if(m_core->setting(KittySDK::Settings::S_ROSTER_TIPS_DESCRIPTION).toBool()) {
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

}
