#include "wndMain.h"
#include "ui_wndMain.h"

#include "KittyCore.h"

wndMain::wndMain(QWidget *parent): QMainWindow(parent), m_ui(new Ui::wndMain)
{
  m_ui->setupUi(this);

  KittyCore *core = KittyCore::inst();
  core->test();
}

wndMain::~wndMain()
{
  delete m_ui;
}
