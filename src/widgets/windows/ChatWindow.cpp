#include "ChatWindow.h"
#include "ui_ChatWindow.h"

#include "SDK/constants.h"
#include "IconManager.h"
#include "../ChatTab.h"
#include "Core.h"

#include <QtCore/QDebug>

using namespace KittySDK;

Kitty::ChatWindow::ChatWindow(QWidget *parent): QWidget(parent), m_ui(new Ui::ChatWindow)
{
  m_ui->setupUi(this);

  connect(IconManager::inst(), SIGNAL(iconsUpdated()), this, SLOT(updateIcons()));

  m_ui->tabWidget->addTab(new Kitty::ChatTab(this), "Tab 1");
  m_ui->tabWidget->addTab(new Kitty::ChatTab(this), "Tab 2");
  m_ui->tabWidget->addTab(new Kitty::ChatTab(this), "Tab 3");

  qDebug() << "Creating ChatWindow";

  Core *core = Core::inst();

  restoreGeometry(core->setting(Settings::S_CHATWINDOW_GEOMETRY).toByteArray());
}

Kitty::ChatWindow::~ChatWindow()
{
  Core *core = Core::inst();

  core->setSetting(Settings::S_CHATWINDOW_GEOMETRY, saveGeometry());

  delete m_ui;
}

void Kitty::ChatWindow::updateIcons()
{
  for(int i = 0; i < m_ui->tabWidget->count(); i++) {
    Kitty::ChatTab *tab = qobject_cast<Kitty::ChatTab*>(m_ui->tabWidget->widget(i));
    tab->updateIcons();
  }
}

void Kitty::ChatWindow::on_tabWidget_tabCloseRequested(int index)
{
  delete m_ui->tabWidget->widget(index);
}
