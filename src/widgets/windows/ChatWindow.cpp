#include "ChatWindow.h"
#include "ui_ChatWindow.h"

#include "../ChatTab.h"

#include <QtCore/QDebug>

Kitty::ChatWindow::ChatWindow(QWidget *parent): QWidget(parent), m_ui(new Ui::ChatWindow)
{
  m_ui->setupUi(this);

  m_ui->tabWidget->addTab(new Kitty::ChatTab(this), "Tab 1");
  m_ui->tabWidget->addTab(new Kitty::ChatTab(this), "Tab 2");
  m_ui->tabWidget->addTab(new Kitty::ChatTab(this), "Tab 3");

  qDebug() << "Creating ChatWindow";
}

Kitty::ChatWindow::~ChatWindow()
{
  delete m_ui;
}

void Kitty::ChatWindow::on_tabWidget_tabCloseRequested(int index)
{
  m_ui->tabWidget->removeTab(index);
  delete m_ui->tabWidget->widget(index);
}
