#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtCore/QTimer>
#include <QtGui/QMainWindow>

class QToolButton;

namespace Ui
{
  class MainWindow;
}

namespace KittySDK
{
  class Contact;
}

namespace Kitty
{
  class RosterItemModel;
  class RosterSortProxy;
  class RosterHeader;

  class MainWindow: public QMainWindow
  {
    Q_OBJECT

    public:
      explicit MainWindow(QWidget *parent = 0);
      ~MainWindow();

      bool isObscured();

      void initToolbars();
      void addToolbarAction(const QString &tb, QAction *action);

      QToolButton *buttonForAction(const QString &tb, QAction *action);

    public slots:
      void addContact(KittySDK::Contact *contact);
      void applySettings();
      void showAccountStatusMenu();
      void updateAccountStatusIcon();

    private slots:
      void showToolbarMenu(QPoint pos);
      void setToolbarIconsOnly();
      void setToolbarTextOnly();
      void setToolbarTextBeside();
      void setToolbarTextUnder();
      void setFilterText(const QString &text);
      void toggleToolbarLock();

    protected:
      void changeEvent(QEvent *event);
      void enterEvent(QEvent *event);
      void leaveEvent(QEvent *event);
      bool eventFilter(QObject *obj, QEvent *event);

    private:
      QTimer m_hideTimer;
      Ui::MainWindow *m_ui;
      RosterHeader *m_header;
      RosterItemModel *m_model;
      RosterSortProxy *m_proxy;
  };
}

#endif // MAINWINDOW_H
