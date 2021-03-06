#ifndef ROSTERITEM_H
#define ROSTERITEM_H

#include <QtGui/QStandardItem>

namespace Kitty
{
	class RosterItem: public QStandardItem
	{
		public:
			enum ItemType
			{
				Group = 0,
				Contact
			};

			enum
			{
				TypeRole = Qt::UserRole + 1,
				AccountRole,
				ProtocolRole,
				UidRole,
				StatusRole,
				DescriptionRole,
				AvatarRole,
				TemporaryRole,
				ExpandedRole
			};

		public:
			RosterItem(RosterItem *parent = 0);
			~RosterItem();

			void appendChild(RosterItem *child);
			void removeChild(RosterItem *child);

			RosterItem *child(int row);
			int childCount() const;
			int row() const;
			RosterItem *parent();

		private:
			QList<RosterItem*> m_children;
			RosterItem *m_parent;
	};
}
#endif
