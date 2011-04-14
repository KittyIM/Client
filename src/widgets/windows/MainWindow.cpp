#include "MainWindow.h"
#include "ui_MainWindow.h"

#include "RosterItemModel.h"
#include "RosterSortProxy.h"
#include "ContactManager.h"
#include "AccountManager.h"
#include "SDK/constants.h"
#include "RosterContact.h"
#include "RosterTheme.h"
#include "constants.h"
#include "Profile.h"
#include "Core.h"

#ifdef Q_WS_WIN32
#include <qt_windows.h>
#endif

#include <QtCore/QDebug>
#include <QtCore/QTimer>
#include <QtGui/QLinearGradient>
#include <QtGui/QToolButton>
#include <QtGui/QMenu>

using namespace KittySDK;

Kitty::MainWindow::MainWindow(QWidget *parent): QMainWindow(parent), m_ui(new Ui::MainWindow)
{
  m_ui->setupUi(this);

  setWindowFlags(windowFlags() | Qt::Tool);
  qDebug() << "Creating MainWindow";

  m_model = new Kitty::RosterItemModel(m_ui->rosterTreeView);
  m_proxy = new Kitty::RosterSortProxy(m_ui->rosterTreeView);
  m_proxy->setSourceModel(m_model);

  m_ui->rosterTreeView->setModel(m_proxy);
  m_ui->rosterTreeView->setSortingEnabled(true);

  connect(m_proxy, SIGNAL(layoutChanged()), m_ui->rosterTreeView, SLOT(fixGroups()));

  m_hideTimer.setSingleShot(true);
  connect(&m_hideTimer, SIGNAL(timeout()), this, SLOT(hide()));

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

Kitty::MainWindow::~MainWindow()
{
  Core *core = Core::inst();

  core->setSetting(Settings::S_MAINWINDOW_STATE, saveState());
  core->setSetting(Settings::S_MAINWINDOW_GEOMETRY, saveGeometry());

  delete m_ui;
}

bool Kitty::MainWindow::isObscured()
{
#ifdef Q_WS_WIN32
  QList<HWND> visited;

  RECT rect;
  GetWindowRect(winId(), &rect);

  HWND hWnd = GetWindow(winId(), GW_HWNDPREV);
  while(hWnd) {
    if(visited.contains(hWnd)) {
      break;
    }

    visited.append(hWnd);

    if(IsWindowVisible(hWnd)) {
      RECT rect2;

      if(GetWindowRect(hWnd, &rect2)) {
        RECT intersect;

        if(IntersectRect(&intersect, &rect, &rect2)) {
          return true;
        }
      }
    }

    hWnd = GetWindow(hWnd, GW_HWNDPREV);
  }
#endif

  return false;
}

void Kitty::MainWindow::initToolbars()
{
  qDebug() << "  Initializing toolbars";

  Core *core = Core::inst();

  QMenu *mnuMain = new QMenu(this);
  mnuMain->addAction(core->action(Actions::A_ABOUT));
  mnuMain->addAction(core->action(Actions::A_DEBUG));
  mnuMain->addSeparator();
  mnuMain->addAction(core->action(Actions::A_KITTY_FOLDER));
  mnuMain->addAction(core->action(Actions::A_PROFILE_FOLDER));
  mnuMain->addSeparator();
  mnuMain->addAction(core->action(Actions::A_RESTART));
  mnuMain->addAction(core->action(Actions::A_QUIT));

  m_ui->mainToolBar->addAction(core->action(Actions::A_ABOUT));

  QToolButton *btnMain = qobject_cast<QToolButton*>(m_ui->mainToolBar->widgetForAction(core->action(Actions::A_ABOUT)));
  btnMain->setMenu(mnuMain);
  btnMain->setPopupMode(QToolButton::MenuButtonPopup);


  QMenu *mnuUser = new QMenu(this);
  //mnuUser->addAction(core->getAction(Actions::A_OPEN_KITTY_FOLDER));

  QToolButton *btnUser = new QToolButton();
  btnUser->setObjectName("userButton");
  btnUser->setText(tr("User"));
  btnUser->setMenu(mnuUser);
  btnUser->setPopupMode(QToolButton::MenuButtonPopup);
  m_ui->mainToolBar->addWidget(btnUser);


  QMenu *mnuSettings = new QMenu(this);
  mnuSettings->addAction(core->action(Actions::A_SETTINGS));

  m_ui->mainToolBar->addAction(core->action(Actions::A_SETTINGS));

  QToolButton *btnSettings = qobject_cast<QToolButton*>(m_ui->mainToolBar->widgetForAction(core->action(Actions::A_SETTINGS)));
  btnSettings->setMenu(mnuSettings);
  btnSettings->setPopupMode(QToolButton::MenuButtonPopup);
}

void Kitty::MainWindow::addToolbarAction(const QString &tb, QAction *action)
{
  if(tb == Toolbars::TB_MAIN) {
    m_ui->mainToolBar->addAction(action);
  } else if(tb == Toolbars::TB_NETWORKS) {
    m_ui->networksToolBar->addAction(action);
  } else if(tb == Toolbars::TB_PLUGINS) {
    m_ui->pluginsToolBar->addAction(action);
  } else {
    qWarning() << "Unknown ToolBar" << tb;
  }
}

void Kitty::MainWindow::loadContacts()
{
  QList<KittySDK::Contact*> contacts = Kitty::ContactManager::inst()->contacts();
  foreach(KittySDK::Contact *cnt, contacts) {
    Kitty::RosterContact *contact = new Kitty::RosterContact(cnt, m_model->groupItem(cnt->group()));

    m_model->addContact(contact, m_model->groupItem(cnt->group()));
  }

  m_proxy->invalidate();
}

void Kitty::MainWindow::applySettings()
{
  Core *core = Core::inst();

  if(core->setting(Settings::S_MAINWINDOW_TRANSPARENCY).toBool()) {
    setWindowOpacity(core->setting(Settings::S_MAINWINDOW_TRANSPARENCY_VALUE, 80).toReal() / 100.0);
  } else {
    setWindowOpacity(1.0);
  }

  if(core->setting(Settings::S_MAINWINDOW_ALWAYS_ON_TOP).toBool()) {
    setWindowFlags(windowFlags() | Qt::WindowStaysOnTopHint);
    show();
  } else {
    bool visible = isVisible();
    setWindowFlags(windowFlags() & ~Qt::WindowStaysOnTopHint);
    setVisible(visible);
  }

  m_hideTimer.setInterval(core->setting(Settings::S_MAINWINDOW_AUTOHIDE_DELAY, 5).toInt() * 1000);

  m_proxy->invalidate();

  QString title = core->setting(Settings::S_MAINWINDOW_CAPTION, "KittyIM %version% [%profile%]").toString();
  title.replace("%version%", Constants::VERSION);
  title.replace("%profile%", core->profile()->name());
  setWindowTitle(title);
}

void Kitty::MainWindow::showAccountStatusMenu()
{
  QAction *action = qobject_cast<QAction*>(sender());
  KittySDK::Account *account = Kitty::AccountManager::inst()->account(action->property("protocol").toString(), action->property("uid").toString());
  if(account) {
    QMenu *menu = account->statusMenu();
    if(menu) {
      QToolButton *button = qobject_cast<QToolButton*>(m_ui->networksToolBar->widgetForAction(action));

      menu->exec(button->mapToGlobal(QPoint(0, button->height() + 1)));
    }
  }
}

void Kitty::MainWindow::updateAccountStatusIcon()
{
  KittySDK::Account *account = dynamic_cast<KittySDK::Account*>(sender());
  foreach(QAction *action, m_ui->networksToolBar->actions()) {
    if((action->property("protocol").toString() == account->protocol()->protoInfo()->protoName()) && (action->property("uid").toString() == account->uid())) {
      action->setIcon(Kitty::Core::inst()->icon(account->protocol()->statusIcon(account->status())));
    }
  }
}

void Kitty::MainWindow::changeEvent(QEvent *event)
{
  if(event->type() == QEvent::LanguageChange) {
    m_ui->retranslateUi(this);
  }

  QMainWindow::changeEvent(event);
}

void Kitty::MainWindow::enterEvent(QEvent *event)
{
  QMainWindow::enterEvent(event);

  m_hideTimer.stop();
}

void Kitty::MainWindow::leaveEvent(QEvent *event)
{
  QMainWindow::leaveEvent(event);

  if(Core::inst()->setting(Settings::S_MAINWINDOW_AUTOHIDE, false).toBool()) {
    m_hideTimer.start();
  }
}

