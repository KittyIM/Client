#ifndef ROSTERITEM_H
#define ROSTERITEM_H

#include <QStandardItem>

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
        DescriptionRole,
        AvatarRole,
        ChildrenRole
      };

      enum { Type = UserType + 1 };
      int type() const { return Type; }

    public:
      RosterItem(RosterItem *parent = 0);
      ~RosterItem();

      void appendChild(RosterItem *child);

      RosterItem *child(int row);
      int childCount() const;
      int columnCount() const;
      int row() const;
      RosterItem *parent();

    private:
      QList<RosterItem*> m_children;
      RosterItem *m_parent;
  };
}
#endif
