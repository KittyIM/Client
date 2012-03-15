#ifndef ROSTERCONTACT_H
#define ROSTERCONTACT_H

#include "RosterItem.h"

namespace KittySDK
{
	class IContact;
}

namespace Kitty
{
	class Core;

	class RosterContact: public RosterItem
	{
		public:
			enum
			{
				BlinkRole = ExpandedRole + 1
			};

		public:
			RosterContact(KittySDK::IContact *contact, Core *core, RosterItem *parent = 0);

			KittySDK::IContact *contact() const { return m_contact; }

			QVariant data(int role = Qt::UserRole + 1) const;


		private:
			KittySDK::IContact *m_contact;
			Core *m_core;
	};
}

#endif // ROSTERCONTACT_H
