#ifndef CONTACTWINDOW_H
#define CONTACTWINDOW_H

#include "SDK/Contact.h"

#include <QtGui/QDialog>

class QTreeWidgetItem;

namespace Ui
{
  class ContactWindow;
}

namespace Kitty
{
  class ContactWindow: public QDialog
  {
    Q_OBJECT

    public:
      explicit ContactWindow(KittySDK::Contact *cnt = 0, QWidget *parent = 0);
      ~ContactWindow();

    public slots:
      void applySettings();

    private slots:
      void on_treeWidget_currentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous);

      void on_buttonBox_accepted();

      void on_changeAvatarButton_clicked();

  protected:
      void showEvent(QShowEvent *event);

    private:
      Ui::ContactWindow *m_ui;
      KittySDK::Contact *m_contact;
  };
}

#endif
