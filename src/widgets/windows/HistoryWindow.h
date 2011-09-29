#ifndef HISTORYWINDOW_H
#define HISTORYWINDOW_H

#include "SDK/Contact.h"

#include <QtGui/QSortFilterProxyModel>
#include <QtGui/QWidget>

class QTreeWidgetItem;

namespace Ui
{
  class HistoryWindow;
}

namespace Kitty
{
  class ContactProxy: public QSortFilterProxyModel
  {
    Q_OBJECT

    public:
      explicit ContactProxy(QObject *parent = 0);

    protected:
      bool filterAcceptsRow(int row, const QModelIndex &parent) const;
  };

  class HistoryWindow: public QWidget
  {
    Q_OBJECT

    public:
      enum
      {
        ItemFolder = 1,
        ItemAccount,
        ItemContact
      };

      enum
      {
        RoleType = Qt::UserRole + 1,
        RoleProtocol,
        RoleAccount,
        RolePathName
      };

    public:
      explicit HistoryWindow(QWidget *parent = 0);
      ~HistoryWindow();

    public slots:
      void applySettings();
      void updateIcons();
      void showContact(KittySDK::Contact *contact);

    protected:
      void showEvent(QShowEvent *event);

    private slots:
      void loadChats(const QItemSelection &selected, const QItemSelection &deselected);
      void on_contactSearchEdit_textChanged(const QString &text);
      void on_chatTree_currentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous);
      void updateCurrentChat();
      void on_filtersButton_toggled(bool checked);
      void on_chatTree_doubleClicked(const QModelIndex &index);

    private:
      QModelIndex findContact(KittySDK::Contact *contact, const QModelIndex &parent = QModelIndex());

    private:
      Ui::HistoryWindow *m_ui;
      class ContactProxy *m_proxy;
  };
}

#endif // HISTORYWINDOW_H
