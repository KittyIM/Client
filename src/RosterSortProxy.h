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

      void setGroupFilter(const QString &groupFilter) { m_groupFilter = groupFilter; }
      QString groupFilter() { return m_groupFilter; }

    private:
      bool lessThan(const QModelIndex &left, const QModelIndex &right) const;
      bool filterAcceptsRow(int source_row, const QModelIndex& source_parent) const;

    private:
      QString m_groupFilter;
  };
}

#endif // ROSTERSORTPROXY_H
