#include "MainWindow.h"
#include "ui_MainWindow.h"

#include "widgets/RosterHeader.h"
#include "RosterItemModel.h"
#include "RosterSortProxy.h"
#include "ContactManager.h"
#include "AccountManager.h"
#include "SDK/constants.h"
#include "RosterContact.h"
#include "constants.h"
#include "Profile.h"
#include "Core.h"

#ifdef Q_WS_WIN32
#include <qt_windows.h>
#endif

#include <QtCore/QDebug>
#include <QtCore/QTimer>
#include <QtCore/QFile>
#include <QtGui/QLinearGradient>
#include <QtGui/QToolButton>
#include <QtGui/QMenu>
#include <QtGui/QMessageBox>

#define qDebug() qDebug() << "[MainWindow]"
#define qWarning() qWarning() << "[MainWindow]"

using namespace Kitty;
using namespace KittySDK;

Kitty::MainWindow::MainWindow(QWidget *parent): QMainWindow(parent), m_ui(new Ui::MainWindow)
{
  m_ui->setupUi(this);

  setWindowFlags(windowFlags() | Qt::Tool);

  qDebug() << "Creating";

  m_header = new RosterHeader(this);
  m_ui->headerToolBar->addWidget(m_header);
  connect(m_header, SIGNAL(descriptionChanged(QString)), AccountManager::inst(), SLOT(changeDescription(QString)));

  m_model = new RosterItemModel(m_ui->rosterTreeView);
  m_proxy = new RosterSortProxy(m_ui->rosterTreeView);
  m_proxy->setSourceModel(m_model);

  connect(ContactManager::inst(), SIGNAL(contactAdded(KittySDK::Contact*)), this, SLOT(addContact(KittySDK::Contact*)));

  m_ui->rosterTreeView->setModel(m_proxy);
  m_ui->rosterTreeView->setSortingEnabled(true);

  connect(m_proxy, SIGNAL(layoutChanged()), m_ui->rosterTreeView, SLOT(fixGroups()));

  m_hideTimer.setSingleShot(true);
  connect(&m_hideTimer, SIGNAL(timeout()), this, SLOT(hide()));

  Core *core = Core::inst();

  core->showTrayIcon();

  initToolbars();

  connect(m_ui->mainToolBar, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(showToolbarMenu(QPoint)));
  connect(m_ui->networksToolBar, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(showToolbarMenu(QPoint)));
  connect(m_ui->pluginsToolBar, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(showToolbarMenu(QPoint)));

  restoreState(core->setting(Settings::S_MAINWINDOW_STATE).toByteArray());
  restoreGeometry(core->setting(Settings::S_MAINWINDOW_GEOMETRY).toByteArray());

  m_ui->mainToolBar->setToolButtonStyle((Qt::ToolButtonStyle)core->setting(QString("%1.%2").arg(Settings::S_MAINWINDOW_TB_STYLES).arg(Toolbars::TB_MAIN), Qt::ToolButtonIconOnly).toInt());
  m_ui->networksToolBar->setToolButtonStyle((Qt::ToolButtonStyle)core->setting(QString("%1.%2").arg(Settings::S_MAINWINDOW_TB_STYLES).arg(Toolbars::TB_NETWORKS), Qt::ToolButtonIconOnly).toInt());
  m_ui->pluginsToolBar->setToolButtonStyle((Qt::ToolButtonStyle)core->setting(QString("%1.%2").arg(Settings::S_MAINWINDOW_TB_STYLES).arg(Toolbars::TB_PLUGINS), Qt::ToolButtonIconOnly).toInt());

  m_ui->mainToolBar->setMovable(core->setting(QString("%1.%2").arg(Settings::S_MAINWINDOW_TB_LOCKS).arg(Toolbars::TB_MAIN), true).toBool());
  m_ui->networksToolBar->setMovable(core->setting(QString("%1.%2").arg(Settings::S_MAINWINDOW_TB_LOCKS).arg(Toolbars::TB_NETWORKS), true).toBool());
  m_ui->pluginsToolBar->setMovable(core->setting(QString("%1.%2").arg(Settings::S_MAINWINDOW_TB_LOCKS).arg(Toolbars::TB_PLUGINS), true).toBool());

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

  core->setSetting(QString("%1.%2").arg(Settings::S_MAINWINDOW_TB_STYLES).arg(Toolbars::TB_MAIN), m_ui->mainToolBar->toolButtonStyle());
  core->setSetting(QString("%1.%2").arg(Settings::S_MAINWINDOW_TB_STYLES).arg(Toolbars::TB_NETWORKS), m_ui->networksToolBar->toolButtonStyle());
  core->setSetting(QString("%1.%2").arg(Settings::S_MAINWINDOW_TB_STYLES).arg(Toolbars::TB_PLUGINS), m_ui->pluginsToolBar->toolButtonStyle());

  core->setSetting(QString("%1.%2").arg(Settings::S_MAINWINDOW_TB_LOCKS).arg(Toolbars::TB_MAIN), m_ui->mainToolBar->isMovable());
  core->setSetting(QString("%1.%2").arg(Settings::S_MAINWINDOW_TB_LOCKS).arg(Toolbars::TB_NETWORKS), m_ui->networksToolBar->isMovable());
  core->setSetting(QString("%1.%2").arg(Settings::S_MAINWINDOW_TB_LOCKS).arg(Toolbars::TB_PLUGINS), m_ui->pluginsToolBar->isMovable());

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
  qDebug() << "Initializing toolbars";

  Core *core = Core::inst();

  // Main menu
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


  // User menu
  QMenu *mnuUser = new QMenu(this);
  mnuUser->addAction(core->action(Actions::A_ADD_CONTACT));
  mnuUser->addAction(core->action(Actions::A_HISTORY));

  m_ui->mainToolBar->addAction(core->action(Actions::A_ADD_CONTACT));

  QToolButton *btnUser = qobject_cast<QToolButton*>(m_ui->mainToolBar->widgetForAction(core->action(Actions::A_ADD_CONTACT)));
  btnUser->setMenu(mnuUser);
  btnUser->setPopupMode(QToolButton::MenuButtonPopup);


  //Settings menu
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

void Kitty::MainWindow::applySettings()
{
  Core *core = Core::inst();

  m_header->applySettings();

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

  QString fileName;
  if(core->setting(Settings::S_ROSTER_THEME, QString()).toString().isEmpty()) {
    fileName = ":/roster/theme.css";
  } else {
    fileName = qApp->applicationDirPath() + "/themes/roster/" + core->setting(Settings::S_ROSTER_THEME, QString()).toString() + "/theme.css";
  }

  QFile file(fileName);
  if(file.open(QFile::ReadOnly)) {
    m_ui->rosterTreeView->setStyleSheet(file.readAll());
    file.close();
  }

  m_proxy->invalidate();

  QString title = core->setting(Settings::S_MAINWINDOW_CAPTION, "KittyIM %version% [%profile%]").toString();
  title.replace("%version%", Constants::VERSION);
  title.replace("%profile%", core->profile()->name());
  setWindowTitle(title);
}

void Kitty::MainWindow::showAccountStatusMenu()
{
  QAction *action = qobject_cast<QAction*>(sender());
  Account *account = AccountManager::inst()->account(action->property("protocol").toString(), action->property("uid").toString());
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
  Account *account = dynamic_cast<Account*>(sender());
  foreach(QAction *action, m_ui->networksToolBar->actions()) {
    if((action->property("protocol").toString() == account->protocol()->protoInfo()->protoName()) && (action->property("uid").toString() == account->uid())) {
      action->setIcon(Core::inst()->icon(account->protocol()->statusIcon(account->status())));
    }
  }
}

void Kitty::MainWindow::showToolbarMenu(QPoint pos)
{
  QToolBar *toolbar = qobject_cast<QToolBar*>(sender());

  if(toolbar) {
    QMenu menu(this);

    QAction *lock = menu.addAction(tr("Lock"), this, SLOT(toggleToolbarLock()));
    lock->setProperty("toolbar", toolbar->objectName());
    lock->setCheckable(true);
    lock->setChecked(!toolbar->isMovable());

    menu.addSeparator();

    QAction *iconsOnly = menu.addAction(tr("Icons only"), this, SLOT(setToolbarIconsOnly()));
    iconsOnly->setProperty("toolbar", toolbar->objectName());
    iconsOnly->setCheckable(true);
    iconsOnly->setChecked(toolbar->toolButtonStyle() == Qt::ToolButtonIconOnly);

    QAction *textOnly = menu.addAction(tr("Text only"), this, SLOT(setToolbarTextOnly()));
    textOnly->setProperty("toolbar", toolbar->objectName());
    textOnly->setCheckable(true);
    textOnly->setChecked(toolbar->toolButtonStyle() == Qt::ToolButtonTextOnly);

    QAction *textBeside = menu.addAction(tr("Text beside icon"), this, SLOT(setToolbarTextBeside()));
    textBeside->setProperty("toolbar", toolbar->objectName());
    textBeside->setCheckable(true);
    textBeside->setChecked(toolbar->toolButtonStyle() == Qt::ToolButtonTextBesideIcon);

    QAction *textUnder = menu.addAction(tr("Text under icon"), this, SLOT(setToolbarTextUnder()));
    textUnder->setProperty("toolbar", toolbar->objectName());
    textUnder->setCheckable(true);
    textUnder->setChecked(toolbar->toolButtonStyle() == Qt::ToolButtonTextUnderIcon);

    menu.exec(toolbar->mapToGlobal(pos));
  }
}

void Kitty::MainWindow::setToolbarIconsOnly()
{
  QAction *action = qobject_cast<QAction*>(sender());
  if(action) {
    QToolBar *toolbar = findChild<QToolBar*>(action->property("toolbar").toString());
    if(toolbar) {
      toolbar->setToolButtonStyle(Qt::ToolButtonIconOnly);
    }
  }
}

void Kitty::MainWindow::setToolbarTextOnly()
{
  QAction *action = qobject_cast<QAction*>(sender());
  if(action) {
    QToolBar *toolbar = findChild<QToolBar*>(action->property("toolbar").toString());
    if(toolbar) {
      toolbar->setToolButtonStyle(Qt::ToolButtonTextOnly);
    }
  }
}

void Kitty::MainWindow::setToolbarTextBeside()
{
  QAction *action = qobject_cast<QAction*>(sender());
  if(action) {
    QToolBar *toolbar = findChild<QToolBar*>(action->property("toolbar").toString());
    if(toolbar) {
      toolbar->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    }
  }
}

void Kitty::MainWindow::setToolbarTextUnder()
{
  QAction *action = qobject_cast<QAction*>(sender());
  if(action) {
    QToolBar *toolbar = findChild<QToolBar*>(action->property("toolbar").toString());
    if(toolbar) {
      toolbar->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    }
  }
}

void Kitty::MainWindow::toggleToolbarLock()
{
  QAction *action = qobject_cast<QAction*>(sender());
  if(action) {
    QToolBar *toolbar = findChild<QToolBar*>(action->property("toolbar").toString());
    if(toolbar) {
      toolbar->setMovable(!toolbar->isMovable());
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

void Kitty::MainWindow::addContact(KittySDK::Contact *contact)
{
  RosterContact *cnt = new RosterContact(contact, m_model->groupItem(contact->group()));

  m_model->addContact(cnt, m_model->groupItem(contact->group()));
  m_proxy->invalidate();
}
