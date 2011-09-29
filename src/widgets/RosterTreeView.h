#ifndef ROSTERTREEVIEW_H
#define ROSTERTREEVIEW_H

#include <QtCore/QPointer>
#include <QtGui/QTreeView>

namespace KittySDK
{
  class Contact;
}

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
      void showHistory();

    private slots:
      void itemExpanded(const QModelIndex &index);
      void itemCollapsed(const QModelIndex &index);

    signals:
      void vCardRequested(KittySDK::Contact *cnt);
      void historyRequested(KittySDK::Contact *cnt);

    protected:
      void mousePressEvent(QMouseEvent *event);
      void mouseDoubleClickEvent(QMouseEvent *event);
      void keyPressEvent(QKeyEvent *event);
  };
}

#endif // ROSTERTREEVIEW_H
