#ifndef ROSTERITEMMODEL_H
#define ROSTERITEMMODEL_H

#include <QtCore/QAbstractItemModel>

namespace Kitty
{
  class RosterItem;

  class RosterItemModel: public QAbstractItemModel
  {
    Q_OBJECT

    public:
      RosterItemModel(QObject *parent = 0);
      ~RosterItemModel();

      RosterItem *addGroup(const QString &name);
      RosterItem *addContact(const QString &name, RosterItem *parent = 0);

      QVariant data(const QModelIndex &index, int role) const;
      QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const;
      QModelIndex parent(const QModelIndex &index) const;
      int rowCount(const QModelIndex &parent = QModelIndex()) const;
      int columnCount(const QModelIndex &parent = QModelIndex()) const;

    private:
      RosterItem *m_root;
  };
}

#endif
