#ifndef ROSTERCONTACT_H
#define ROSTERCONTACT_H

#include "RosterItem.h"

namespace KittySDK
{
	class IContact;
}

namespace Kitty
{
	class RosterContact: public RosterItem
	{
		public:
			enum
			{
				BlinkRole = ExpandedRole + 1
			};

		public:
			explicit RosterContact(KittySDK::IContact *contact, RosterItem *parent = 0);

			KittySDK::IContact *contact() const { return m_contact; }

			QVariant data(int role = Qt::UserRole + 1) const;


		private:
			KittySDK::IContact *m_contact;
	};
}

#endif // ROSTERCONTACT_H
