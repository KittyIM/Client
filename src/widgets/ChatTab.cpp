#include "ChatTab.h"
#include "ui_ChatTab.h"

Kitty::ChatTab::ChatTab(QWidget *parent): QWidget(parent), m_ui(new Ui::ChatTab)
{
  m_ui->setupUi(this);
}

Kitty::ChatTab::~ChatTab()
{
  delete m_ui;
}
