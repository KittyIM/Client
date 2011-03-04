#include "MainWindow.h"
#include "ui_MainWindow.h"

#include "SDK/constants.h"
#include "Core.h"

#include <QtCore/QDebug>
#include <QtGui/QToolButton>
#include <QtGui/QMenu>

using namespace Kitty;

MainWindow::MainWindow(QWidget *parent): QMainWindow(parent), m_ui(new Ui::MainWindow)
{
  m_ui->setupUi(this);

  setWindowFlags(windowFlags() | Qt::Tool);

  Core *core = Core::inst();

  core->showTrayIcon();

  initToolbars();

  restoreState(core->setting("Kitty.MainWindow.State").toByteArray());
  restoreGeometry(core->setting("Kitty.MainWindow.Geometry").toByteArray());

  applySettings();

  if(!core->setting("Kitty.MainWindow.StartHidden").toBool()) {
    show();
  }
}

MainWindow::~MainWindow()
{
  Core *core = Core::inst();

  core->setSetting("Kitty.MainWindow.State", saveState());
  core->setSetting("Kitty.MainWindow.Geometry", saveGeometry());


  delete m_ui;
}

void MainWindow::initToolbars()
{
  Core *core = Core::inst();

  QMenu *mnuMain = new QMenu();
  mnuMain->addAction(core->action(KittySDK::Actions::A_ABOUT));
  mnuMain->addAction(core->action(KittySDK::Actions::A_DEBUG));
  mnuMain->addSeparator();
  mnuMain->addAction(core->action(KittySDK::Actions::A_KITTY_FOLDER));
  mnuMain->addAction(core->action(KittySDK::Actions::A_PROFILE_FOLDER));
  mnuMain->addSeparator();
  mnuMain->addAction(core->action(KittySDK::Actions::A_RESTART));
  mnuMain->addAction(core->action(KittySDK::Actions::A_QUIT));

  QToolButton *btnMain = new QToolButton();
  btnMain->setText("KittyIM");
  btnMain->setIcon(core->icon(KittySDK::Icons::I_INFO));
  btnMain->setMenu(mnuMain);
  btnMain->setPopupMode(QToolButton::MenuButtonPopup);
  connect(btnMain, SIGNAL(clicked()), core->action(KittySDK::Actions::A_ABOUT), SIGNAL(triggered()));
  m_ui->mainToolBar->addWidget(btnMain);


  QMenu *mnuUser = new QMenu();
  //mnuUser->addAction(core->getAction(KittySDK::Actions::A_OPEN_KITTY_FOLDER));

  QToolButton *btnUser = new QToolButton();
  btnUser->setText(tr("User"));
  btnUser->setIcon(core->icon(KittySDK::Icons::I_USER));
  btnUser->setMenu(mnuUser);
  btnUser->setPopupMode(QToolButton::MenuButtonPopup);
  m_ui->mainToolBar->addWidget(btnUser);


  QMenu *mnuSettings = new QMenu();
  mnuSettings->addAction(core->action(KittySDK::Actions::A_SETTINGS));

  QToolButton *btnSettings = new QToolButton();
  btnSettings->setText(tr("Settings"));
  btnSettings->setIcon(core->icon(KittySDK::Icons::I_SETTINGS));
  btnSettings->setMenu(mnuSettings);
  btnSettings->setPopupMode(QToolButton::MenuButtonPopup);
  m_ui->mainToolBar->addWidget(btnSettings);
}

void MainWindow::applySettings()
{
  Core *core = Core::inst();

  if(core->setting(KittySDK::Settings::S_MAINWINDOW_TRANSPARENCY, false).toBool()) {
    qDebug() << core->setting(KittySDK::Settings::S_MAINWINDOW_TRANSPARENCY_VALUE, 80).toReal() / 100.0;
    setWindowOpacity(core->setting(KittySDK::Settings::S_MAINWINDOW_TRANSPARENCY_VALUE, 80).toReal() / 100.0);
  } else {
    setWindowOpacity(1.0);
  }
}
