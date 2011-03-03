#include "wndMain.h"
#include "ui_wndMain.h"

#include "KittyCore.h"
#include "SDK/constants.h"

#include <QtCore/QDebug>
#include <QtGui/QMenu>
#include <QtGui/QToolButton>
#include <QtWebKit>

wndMain::wndMain(QWidget *parent): QMainWindow(parent), m_ui(new Ui::wndMain)
{
  m_ui->setupUi(this);

  setWindowFlags(windowFlags() | Qt::Tool);

  KittyCore::inst()->showTrayIcon();

  initToolbars();
}

wndMain::~wndMain()
{
  delete m_ui;
}

void wndMain::initToolbars()
{
  KittyCore *core = KittyCore::inst();

  QMenu *mnuMain = new QMenu();
  mnuMain->addAction(core->action(KittySDK::Actions::ABOUT));
  mnuMain->addAction(core->action(KittySDK::Actions::DEBUG));
  mnuMain->addSeparator();
  mnuMain->addAction(core->action(KittySDK::Actions::OPEN_KITTY_FOLDER));
  mnuMain->addAction(core->action(KittySDK::Actions::OPEN_PROFILE_FOLDER));
  mnuMain->addSeparator();
  mnuMain->addAction(core->action(KittySDK::Actions::RESTART));
  mnuMain->addAction(core->action(KittySDK::Actions::QUIT));

  QToolButton *btnMain = new QToolButton();
  btnMain->setText("KittyIM");
  btnMain->setIcon(core->icon(KittySDK::Icons::INFO));
  btnMain->setMenu(mnuMain);
  btnMain->setPopupMode(QToolButton::MenuButtonPopup);
  connect(btnMain, SIGNAL(clicked()), core->action(KittySDK::Actions::ABOUT), SIGNAL(triggered()));
  m_ui->mainToolBar->addWidget(btnMain);


  QMenu *mnuUser = new QMenu();
  //mnuUser->addAction(core->getAction(KittySDK::Actions::OPEN_KITTY_FOLDER));

  QToolButton *btnUser = new QToolButton();
  btnUser->setText(tr("User"));
  btnUser->setIcon(core->icon(KittySDK::Icons::USER));
  btnUser->setMenu(mnuUser);
  btnUser->setPopupMode(QToolButton::MenuButtonPopup);
  m_ui->mainToolBar->addWidget(btnUser);


  QMenu *mnuSettings = new QMenu();
  mnuSettings->addAction(core->action(KittySDK::Actions::SETTINGS));

  QToolButton *btnSettings = new QToolButton();
  btnSettings->setText(tr("Settings"));
  btnSettings->setIcon(core->icon(KittySDK::Icons::SETTINGS));
  btnSettings->setMenu(mnuSettings);
  btnSettings->setPopupMode(QToolButton::MenuButtonPopup);
  m_ui->mainToolBar->addWidget(btnSettings);
}
