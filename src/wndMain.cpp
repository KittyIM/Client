#include "wndMain.h"
#include "ui_wndMain.h"

#include "KittyCore.h"

#include <QtCore/QDebug>

wndMain::wndMain(QWidget *parent): QMainWindow(parent), m_ui(new Ui::wndMain)
{
  m_ui->setupUi(this);

  setWindowFlags(windowFlags() | Qt::Tool);

  KittyCore::inst()->showTrayIcon();
}

wndMain::~wndMain()
{
  delete m_ui;
}
