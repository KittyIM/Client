#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui/QMainWindow>

namespace Ui
{
  class MainWindow;
}

namespace Kitty
{
  class MainWindow: public QMainWindow
  {
    Q_OBJECT

    public:
      explicit MainWindow(QWidget *parent = 0);
      ~MainWindow();

      bool isObscured();

      void initToolbars();

    public slots:
      void applySettings();

    protected:
      void changeEvent(QEvent *event);

    private:
      Ui::MainWindow *m_ui;
  };
}

#endif // MAINWINDOW_H
