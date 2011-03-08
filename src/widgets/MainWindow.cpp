#include "MainWindow.h"
#include "ui_MainWindow.h"

#include "SDK/constants.h"
#include "constants.h"
#include "Profile.h"
#include "Core.h"

#include <QtCore/QDebug>
#include <QtGui/QToolButton>
#include <QtGui/QMenu>

using namespace Kitty;
using namespace KittySDK;

MainWindow::MainWindow(QWidget *parent): QMainWindow(parent), m_ui(new Ui::MainWindow)
{
  m_ui->setupUi(this);

  setWindowFlags(windowFlags() | Qt::Tool);

  Core *core = Core::inst();

  core->showTrayIcon();

  initToolbars();

  restoreState(core->setting(Settings::S_MAINWINDOW_STATE).toByteArray());
  restoreGeometry(core->setting(Settings::S_MAINWINDOW_GEOMETRY).toByteArray());

  applySettings();

  if(!core->setting(Settings::S_MAINWINDOW_STARTHIDDEN).toBool()) {
    show();
  }
}

MainWindow::~MainWindow()
{
  Core *core = Core::inst();

  core->setSetting(Settings::S_MAINWINDOW_STATE, saveState());
  core->setSetting(Settings::S_MAINWINDOW_GEOMETRY, saveGeometry());

  delete m_ui;
}

void MainWindow::initToolbars()
{
  Core *core = Core::inst();

  QMenu *mnuMain = new QMenu();
  mnuMain->addAction(core->action(Actions::A_ABOUT));
  mnuMain->addAction(core->action(Actions::A_DEBUG));
  mnuMain->addSeparator();
  mnuMain->addAction(core->action(Actions::A_KITTY_FOLDER));
  mnuMain->addAction(core->action(Actions::A_PROFILE_FOLDER));
  mnuMain->addSeparator();
  mnuMain->addAction(core->action(Actions::A_RESTART));
  mnuMain->addAction(core->action(Actions::A_QUIT));

  QToolButton *btnMain = new QToolButton();
  btnMain->setObjectName("mainButton");
  btnMain->setText("KittyIM");
  //btnMain->setIcon(core->icon(Icons::I_INFO));
  btnMain->setMenu(mnuMain);
  btnMain->setPopupMode(QToolButton::MenuButtonPopup);
  connect(btnMain, SIGNAL(clicked()), core->action(Actions::A_ABOUT), SIGNAL(triggered()));
  m_ui->mainToolBar->addWidget(btnMain);


  QMenu *mnuUser = new QMenu();
  //mnuUser->addAction(core->getAction(Actions::A_OPEN_KITTY_FOLDER));

  QToolButton *btnUser = new QToolButton();
  btnUser->setObjectName("userButton");
  btnUser->setText(tr("User"));
  btnUser->setMenu(mnuUser);
  btnUser->setPopupMode(QToolButton::MenuButtonPopup);
  m_ui->mainToolBar->addWidget(btnUser);


  QMenu *mnuSettings = new QMenu();
  mnuSettings->addAction(core->action(Actions::A_SETTINGS));

  QToolButton *btnSettings = new QToolButton();
  btnSettings->setObjectName("settingsButton");
  btnSettings->setText(tr("Settings"));
  btnSettings->setMenu(mnuSettings);
  btnSettings->setPopupMode(QToolButton::MenuButtonPopup);
  connect(btnSettings, SIGNAL(clicked()), core->action(Actions::A_SETTINGS), SIGNAL(triggered()));
  m_ui->mainToolBar->addWidget(btnSettings);
}

void MainWindow::applySettings()
{
  Core *core = Core::inst();

  m_ui->mainToolBar->findChild<QToolButton*>("mainButton")->setIcon(core->icon(Icons::I_INFO));
  m_ui->mainToolBar->findChild<QToolButton*>("userButton")->setIcon(core->icon(Icons::I_USER));
  m_ui->mainToolBar->findChild<QToolButton*>("settingsButton")->setIcon(core->icon(Icons::I_SETTINGS));

  if(core->setting(Settings::S_MAINWINDOW_TRANSPARENCY, false).toBool()) {
    setWindowOpacity(core->setting(Settings::S_MAINWINDOW_TRANSPARENCY_VALUE, 80).toReal() / 100.0);
  } else {
    setWindowOpacity(1.0);
  }

  core->setSetting(Settings::S_MAINWINDOW_CAPTION, "KittyIM %version% [%profile%]");
  QString title = core->setting(Settings::S_MAINWINDOW_CAPTION, "KittyIM %version% [%profile%]").toString();
  title.replace("%version%", Constants::VERSION);
  title.replace("%profile%", core->profile()->name());
  setWindowTitle(title);
}