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

      void initToolbars();

    public slots:
      void applySettings();

    private:
      Ui::MainWindow *m_ui;
  };
}

#endif // MAINWINDOW_H
