#ifndef ROSTERTREEVIEW_H
#define ROSTERTREEVIEW_H

#include <QtGui/QTreeView>

namespace Kitty
{
  class RosterItemDelegate;

  class RosterTreeView: public QTreeView
  {
    Q_OBJECT

    public:
      explicit RosterTreeView(QWidget *parent = 0);

    private:
      RosterItemDelegate *m_delegate;
  };
}

#endif // ROSTERTREEVIEW_H
