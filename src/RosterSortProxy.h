#ifndef ROSTERSORTPROXY_H
#define ROSTERSORTPROXY_H

#include <QtGui/QSortFilterProxyModel>

namespace Kitty
{
  class RosterSortProxy: public QSortFilterProxyModel
  {
    Q_OBJECT

    public:
      explicit RosterSortProxy(QObject *parent = 0);

    private:
      bool lessThan(const QModelIndex &left, const QModelIndex &right) const;
      bool filterAcceptsRow(int source_row, const QModelIndex& source_parent) const;
  };
}

#endif // ROSTERSORTPROXY_H
