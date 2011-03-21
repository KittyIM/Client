#ifndef ROSTERITEM_H
#define ROSTERITEM_H

#include <QStandardItem>

namespace Kitty
{
  class RosterItem: public QStandardItem
  {
    public:
      enum
      {
        GROUP = 0,
        CONTACT = 1
      };

      enum
      {
        TypeRole = Qt::UserRole + 1,
        DescriptionRole,
        AvatarRole,
        IconRole,
        ChildrenRole
      };

      enum { Type = UserType + 1 };
      int type() const { return Type; }

    public:
      RosterItem(const int &type = RosterItem::GROUP, const QString &text = QString(), RosterItem *parent = 0);
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
