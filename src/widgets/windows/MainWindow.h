#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtCore/QTimer>
#include <QtGui/QMainWindow>

namespace Ui
{
  class MainWindow;
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

      void loadContacts();

    public slots:
      void applySettings();
      void showAccountStatusMenu();
      void updateAccountStatusIcon();

    protected:
      void changeEvent(QEvent *event);
      void enterEvent(QEvent *event);
      void leaveEvent(QEvent *event);

    private:
      QTimer m_hideTimer;
      Ui::MainWindow *m_ui;
      RosterHeader *m_header;
      RosterItemModel *m_model;
      RosterSortProxy *m_proxy;
  };
}

#endif // MAINWINDOW_H
