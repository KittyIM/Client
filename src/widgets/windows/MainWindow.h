#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui/QMainWindow>

namespace Ui
{
  class MainWindow;
}

namespace Kitty
{
  class RosterItemModel;

  class MainWindow: public QMainWindow
  {
    Q_OBJECT

    public:
      explicit MainWindow(QWidget *parent = 0);
      ~MainWindow();

      bool isObscured();

      void initToolbars();
      void addToolbarAction(const QString &tb, QAction *action);

    public slots:
      void applySettings();
      void showAccountStatusMenu();
      void updateAccountStatusIcon();

    protected:
      void changeEvent(QEvent *event);

    private:
      Ui::MainWindow *m_ui;
      RosterItemModel *m_model;
  };
}

#endif // MAINWINDOW_H
