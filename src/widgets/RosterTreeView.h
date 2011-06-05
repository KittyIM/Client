#ifndef ROSTERTREEVIEW_H
#define ROSTERTREEVIEW_H

#include <QtCore/QPointer>
#include <QtGui/QTreeView>

namespace Kitty
{
  class RosterItemDelegate;
  class ContactWindow;

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
      void showVCard();

    private slots:
      void itemExpanded(const QModelIndex &index);
      void itemCollapsed(const QModelIndex &index);

    protected:
      void mousePressEvent(QMouseEvent *event);
      void mouseDoubleClickEvent(QMouseEvent *event);
      void keyPressEvent(QKeyEvent *event);

    private:
      QMap<QString, QPointer<Kitty::ContactWindow> > m_windows;
  };
}

#endif // ROSTERTREEVIEW_H
