#include "PortabilityWindow.h"
#include "ui_PortabilityWindow.h"

Kitty::PortabilityWindow::PortabilityWindow(QWidget *parent): QDialog(parent), m_ui(new Ui::PortabilityWindow)
{
  m_ui->setupUi(this);
}

Kitty::PortabilityWindow::~PortabilityWindow()
{
  delete m_ui;
}
