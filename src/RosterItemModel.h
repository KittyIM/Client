#ifndef ROSTERITEMMODEL_H
#define ROSTERITEMMODEL_H

#include <QtCore/QAbstractItemModel>

namespace Kitty
{
  class RosterItem;
  class RosterContact;

  class RosterItemModel: public QAbstractItemModel
  {
    Q_OBJECT

    public:
      RosterItemModel(QObject *parent = 0);
      ~RosterItemModel();

      RosterItem *addGroup(const QString &name);
      RosterItem *addContact(Kitty::RosterContact *item, RosterItem *parent = 0);
      RosterItem *groupItem(const QString &name);

      QVariant data(const QModelIndex &index, int role) const;
      QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const;
      QModelIndex parent(const QModelIndex &index) const;
      int rowCount(const QModelIndex &parent = QModelIndex()) const;
      int columnCount(const QModelIndex &parent = QModelIndex()) const;

    private slots:
      void updateData();

    private:
      RosterItem *m_root;
  };
}

#endif
