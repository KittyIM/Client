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

    public slots:
      void fixGroups();
      void sendMessage();
      void copyName();
      void copyUid();
      void copyDescription();
      void moveToGroup();

    private slots:
      void itemExpanded(const QModelIndex &index);
      void itemCollapsed(const QModelIndex &index);

    protected:
      void mousePressEvent(QMouseEvent *event);
      void mouseDoubleClickEvent(QMouseEvent *event);

    private:
      RosterItemDelegate *m_delegate;
  };
}

#endif // ROSTERTREEVIEW_H
