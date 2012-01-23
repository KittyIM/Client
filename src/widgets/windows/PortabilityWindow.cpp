#include "PortabilityWindow.h"
#include "ui_PortabilityWindow.h"

#define qDebug() qDebug() << "[PortabilityWindow]"
#define qWarning() qWarning() << "[PortabilityWindow]"

Kitty::PortabilityWindow::PortabilityWindow(QWidget *parent): QDialog(parent), m_ui(new Ui::PortabilityWindow)
{
	m_ui->setupUi(this);
}

Kitty::PortabilityWindow::~PortabilityWindow()
{
	delete m_ui;
}
