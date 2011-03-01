#ifndef WNDMAIN_H
#define WNDMAIN_H

#include <QtGui/QMainWindow>

namespace Ui
{
  class wndMain;
}

class wndMain: public QMainWindow
{
  Q_OBJECT

  public:
    explicit wndMain(QWidget *parent = 0);
    ~wndMain();

    void initToolbars();

  private:
    Ui::wndMain *m_ui;
};

#endif // WNDMAIN_H
