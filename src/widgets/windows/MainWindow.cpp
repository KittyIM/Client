#include "MainWindow.h"
#include "ui_MainWindow.h"

#include "widgets/RosterHeader.h"
#include "RosterItemModel.h"
#include "RosterSortProxy.h"
#include "ContactManager.h"
#include "AccountManager.h"
#include "ActionManager.h"
#include "HistoryWindow.h"
#include "RosterContact.h"
#include "IconManager.h"
#include "constants.h"
#include "Profile.h"
#include "Core.h"

#include <SDKConstants.h>

#ifdef Q_WS_WIN32
#include <qt_windows.h>
#endif

#include <QtCore/QDebug>
#include <QtCore/QTimer>
#include <QtCore/QFile>
#include <QtGui/QLinearGradient>
#include <QtGui/QToolButton>
#include <QtGui/QKeyEvent>
#include <QtGui/QMenu>

#define qDebug() qDebug() << "[MainWindow]"
#define qWarning() qWarning() << "[MainWindow]"

namespace Kitty
{

MainWindow::MainWindow(QWidget *parent): QMainWindow(parent), m_ui(new Ui::MainWindow)
{
	m_ui->setupUi(this);

	setWindowFlags(windowFlags() | Qt::Tool);

	qDebug() << "Creating";

	connect(m_ui->rosterTreeView, SIGNAL(vCardRequested(KittySDK::IContact*)), Core::inst(), SLOT(showContactWindow(KittySDK::IContact*)));
	connect(m_ui->rosterTreeView, SIGNAL(historyRequested(KittySDK::IContact*)), this, SLOT(requestHistory(KittySDK::IContact*)));

	m_header = new RosterHeader(this);
	m_ui->headerToolBar->addWidget(m_header);
	connect(m_header, SIGNAL(descriptionChanged(QString)), AccountManager::inst(), SLOT(changeDescription(QString)));
	connect(m_header, SIGNAL(statusChanged(KittySDK::IProtocol::Status)), AccountManager::inst(), SLOT(changeStatus(KittySDK::IProtocol::Status)));

	m_model = new RosterItemModel(m_ui->rosterTreeView);
	m_proxy = new RosterSortProxy(m_ui->rosterTreeView);
	m_proxy->setSourceModel(m_model);

	m_ui->filterEdit->hide();
	m_ui->filterEdit->installEventFilter(this);
	m_ui->rosterTreeView->installEventFilter(this);
	connect(m_ui->filterEdit, SIGNAL(textChanged(QString)), this, SLOT(setFilterText(QString)));

	connect(AccountManager::inst(), SIGNAL(accountStatusChanged(KittySDK::IAccount*,KittySDK::IProtocol::Status,QString)), this, SLOT(updateAccountStatusIcon(KittySDK::IAccount*,KittySDK::IProtocol::Status,QString)));
	connect(ContactManager::inst(), SIGNAL(contactAdded(KittySDK::IContact*)), this, SLOT(addContact(KittySDK::IContact*)));

	m_ui->rosterTreeView->setModel(m_proxy);
	m_ui->rosterTreeView->setSortingEnabled(true);

	connect(m_proxy, SIGNAL(layoutChanged()), m_ui->rosterTreeView, SLOT(fixGroups()));
	connect(IconManager::inst(), SIGNAL(iconsUpdated()), m_proxy, SLOT(invalidate()));

	m_hideTimer.setSingleShot(true);
	connect(&m_hideTimer, SIGNAL(timeout()), this, SLOT(hide()));

	Core *core = Core::inst();

	core->showTrayIcon();

	initToolbars();

	connect(m_ui->mainToolBar, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(showToolbarMenu(QPoint)));
	connect(m_ui->networksToolBar, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(showToolbarMenu(QPoint)));
	connect(m_ui->pluginsToolBar, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(showToolbarMenu(QPoint)));

	restoreState(core->setting(KittySDK::Settings::S_MAINWINDOW_STATE).toByteArray());
	restoreGeometry(core->setting(KittySDK::Settings::S_MAINWINDOW_GEOMETRY).toByteArray());

	m_ui->mainToolBar->setToolButtonStyle((Qt::ToolButtonStyle)core->setting(QString("%1.%2").arg(KittySDK::Settings::S_MAINWINDOW_TB_STYLES).arg(KittySDK::Toolbars::TB_MAIN), Qt::ToolButtonIconOnly).toInt());
	m_ui->networksToolBar->setToolButtonStyle((Qt::ToolButtonStyle)core->setting(QString("%1.%2").arg(KittySDK::Settings::S_MAINWINDOW_TB_STYLES).arg(KittySDK::Toolbars::TB_NETWORKS), Qt::ToolButtonIconOnly).toInt());
	m_ui->pluginsToolBar->setToolButtonStyle((Qt::ToolButtonStyle)core->setting(QString("%1.%2").arg(KittySDK::Settings::S_MAINWINDOW_TB_STYLES).arg(KittySDK::Toolbars::TB_PLUGINS), Qt::ToolButtonIconOnly).toInt());

	m_ui->mainToolBar->setMovable(core->setting(QString("%1.%2").arg(KittySDK::Settings::S_MAINWINDOW_TB_LOCKS).arg(KittySDK::Toolbars::TB_MAIN), true).toBool());
	m_ui->networksToolBar->setMovable(core->setting(QString("%1.%2").arg(KittySDK::Settings::S_MAINWINDOW_TB_LOCKS).arg(KittySDK::Toolbars::TB_NETWORKS), true).toBool());
	m_ui->pluginsToolBar->setMovable(core->setting(QString("%1.%2").arg(KittySDK::Settings::S_MAINWINDOW_TB_LOCKS).arg(KittySDK::Toolbars::TB_PLUGINS), true).toBool());

	applySettings();

	if(!core->setting(KittySDK::Settings::S_MAINWINDOW_STARTHIDDEN).toBool()) {
		show();
	}
}

MainWindow::~MainWindow()
{
	Core *core = Core::inst();

	core->setSetting(KittySDK::Settings::S_MAINWINDOW_STATE, saveState());
	core->setSetting(KittySDK::Settings::S_MAINWINDOW_GEOMETRY, saveGeometry());

	core->setSetting(QString("%1.%2").arg(KittySDK::Settings::S_MAINWINDOW_TB_STYLES).arg(KittySDK::Toolbars::TB_MAIN), m_ui->mainToolBar->toolButtonStyle());
	core->setSetting(QString("%1.%2").arg(KittySDK::Settings::S_MAINWINDOW_TB_STYLES).arg(KittySDK::Toolbars::TB_NETWORKS), m_ui->networksToolBar->toolButtonStyle());
	core->setSetting(QString("%1.%2").arg(KittySDK::Settings::S_MAINWINDOW_TB_STYLES).arg(KittySDK::Toolbars::TB_PLUGINS), m_ui->pluginsToolBar->toolButtonStyle());

	core->setSetting(QString("%1.%2").arg(KittySDK::Settings::S_MAINWINDOW_TB_LOCKS).arg(KittySDK::Toolbars::TB_MAIN), m_ui->mainToolBar->isMovable());
	core->setSetting(QString("%1.%2").arg(KittySDK::Settings::S_MAINWINDOW_TB_LOCKS).arg(KittySDK::Toolbars::TB_NETWORKS), m_ui->networksToolBar->isMovable());
	core->setSetting(QString("%1.%2").arg(KittySDK::Settings::S_MAINWINDOW_TB_LOCKS).arg(KittySDK::Toolbars::TB_PLUGINS), m_ui->pluginsToolBar->isMovable());

	delete m_ui;
}

bool MainWindow::isObscured()
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

void MainWindow::initToolbars()
{
	qDebug() << "Initializing toolbars";

	Core *core = Core::inst();

	// Main menu
	QMenu *mnuMain = new QMenu(this);
	mnuMain->addAction(core->action(KittySDK::Actions::A_ABOUT));
	mnuMain->addAction(core->action(KittySDK::Actions::A_DEBUG));
	mnuMain->addSeparator();
	mnuMain->addAction(core->action(KittySDK::Actions::A_KITTY_FOLDER));
	mnuMain->addAction(core->action(KittySDK::Actions::A_PROFILE_FOLDER));
	mnuMain->addSeparator();
	mnuMain->addAction(core->action(KittySDK::Actions::A_RESTART));
	mnuMain->addAction(core->action(KittySDK::Actions::A_QUIT));

	m_ui->mainToolBar->addAction(core->action(KittySDK::Actions::A_ABOUT));

	if(QToolButton *btnMain = qobject_cast<QToolButton*>(m_ui->mainToolBar->widgetForAction(core->action(KittySDK::Actions::A_ABOUT)))) {
		btnMain->setMenu(mnuMain);
		btnMain->setPopupMode(QToolButton::MenuButtonPopup);
	}

	// User menu
	QMenu *mnuUser = new QMenu(this);
	mnuUser->addAction(core->action(KittySDK::Actions::A_ADD_CONTACT));
	mnuUser->addAction(core->action(KittySDK::Actions::A_HISTORY));
	mnuUser->addAction(core->action(KittySDK::Actions::A_CHANGE_PROFILE));

	m_ui->mainToolBar->addAction(core->action(KittySDK::Actions::A_ADD_CONTACT));

	if(QToolButton *btnUser = qobject_cast<QToolButton*>(m_ui->mainToolBar->widgetForAction(core->action(KittySDK::Actions::A_ADD_CONTACT)))) {
		btnUser->setMenu(mnuUser);
		btnUser->setPopupMode(QToolButton::MenuButtonPopup);
	}


	//Settings menu
	QMenu *mnuSettings = new QMenu(this);
	mnuSettings->addAction(core->action(KittySDK::Actions::A_SETTINGS));

	m_ui->mainToolBar->addAction(core->action(KittySDK::Actions::A_SETTINGS));

	if(QToolButton *btnSettings = qobject_cast<QToolButton*>(m_ui->mainToolBar->widgetForAction(core->action(KittySDK::Actions::A_SETTINGS)))) {
		btnSettings->setMenu(mnuSettings);
		btnSettings->setPopupMode(QToolButton::MenuButtonPopup);
	}
}

void MainWindow::addToolbarAction(const QString &tb, QAction *action)
{
	if(tb == KittySDK::Toolbars::TB_MAIN) {
		m_ui->mainToolBar->addAction(action);
	} else if(tb == KittySDK::Toolbars::TB_NETWORKS) {
		m_ui->networksToolBar->addAction(action);
	} else if(tb == KittySDK::Toolbars::TB_PLUGINS) {
		m_ui->pluginsToolBar->addAction(action);
	} else {
		qWarning() << "Unknown ToolBar" << tb;
	}
}

QToolButton * MainWindow::buttonForAction(const QString &tb, QAction *action)
{
	if(tb == KittySDK::Toolbars::TB_MAIN) {
		return qobject_cast<QToolButton*>(m_ui->mainToolBar->widgetForAction(action));
	} else if(tb == KittySDK::Toolbars::TB_NETWORKS) {
		return qobject_cast<QToolButton*>(m_ui->networksToolBar->widgetForAction(action));
	} else if(tb == KittySDK::Toolbars::TB_PLUGINS) {
		return qobject_cast<QToolButton*>(m_ui->pluginsToolBar->widgetForAction(action));
	} else {
		qWarning() << "Unknown ToolBar" << tb;
	}

	return 0;
}

void MainWindow::applySettings()
{
	Core *core = Core::inst();

	m_header->applySettings();

	if(core->setting(KittySDK::Settings::S_MAINWINDOW_TRANSPARENCY).toBool()) {
		setWindowOpacity(core->setting(KittySDK::Settings::S_MAINWINDOW_TRANSPARENCY_VALUE, 80).toReal() / 100.0);
	} else {
		setWindowOpacity(1.0);
	}

	if(core->setting(KittySDK::Settings::S_MAINWINDOW_ALWAYS_ON_TOP).toBool()) {
		setWindowFlags(windowFlags() | Qt::WindowStaysOnTopHint);
		show();
	} else {
		bool visible = isVisible();
		setWindowFlags(windowFlags() & ~Qt::WindowStaysOnTopHint);
		setVisible(visible);
	}

	m_hideTimer.setInterval(core->setting(KittySDK::Settings::S_MAINWINDOW_AUTOHIDE_DELAY, 5).toInt() * 1000);

	QString fileName;
	if(core->setting(KittySDK::Settings::S_ROSTER_THEME, QString()).toString().isEmpty()) {
		fileName = ":/roster/theme.css";
	} else {
		fileName = qApp->applicationDirPath() + "/themes/roster/" + core->setting(KittySDK::Settings::S_ROSTER_THEME, QString()).toString() + "/theme.css";
	}

	QFile file(fileName);
	if(file.open(QFile::ReadOnly)) {
		setStyleSheet(file.readAll());
		file.close();
	}

	m_proxy->invalidate();

	QString title = core->setting(KittySDK::Settings::S_MAINWINDOW_CAPTION, "KittyIM %version% [%profile%]").toString();
	title.replace("%version%", Constants::VERSION);
	title.replace("%profile%", core->profileName());
	setWindowTitle(title);
}

void MainWindow::showAccountStatusMenu()
{
	if(QAction *action = qobject_cast<QAction*>(sender())) {
		if(KittySDK::IAccount *account = AccountManager::inst()->account(action->property("protocol").toString(), action->property("uid").toString())) {
			if(QMenu *menu = account->statusMenu()) {
				if(QToolButton *button = qobject_cast<QToolButton*>(m_ui->networksToolBar->widgetForAction(action))) {
					menu->exec(button->mapToGlobal(QPoint(0, button->height() + 1)));
				}
			}
		}
	}
}

void MainWindow::updateAccountStatusIcon(KittySDK::IAccount *account, KittySDK::IProtocol::Status status, const QString &description)
{
	foreach(QAction *action, m_ui->networksToolBar->actions()) {
		if((action->property("protocol").toString() == account->protocol()->protoInfo()->protoName()) && (action->property("uid").toString() == account->uid())) {
			action->setIcon(Core::inst()->icon(account->protocol()->statusIcon(status)));

			if(description.length() > 0) {
				action->setToolTip(QString("%1<br>%2").arg(action->text()).arg(description));
			} else {
				action->setToolTip(action->text());
			}
		}
	}
}

void MainWindow::showToolbarMenu(QPoint pos)
{
	if(QToolBar *toolbar = qobject_cast<QToolBar*>(sender())) {
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

void MainWindow::setToolbarIconsOnly()
{
	QAction *action = qobject_cast<QAction*>(sender());
	if(action) {
		QToolBar *toolbar = findChild<QToolBar*>(action->property("toolbar").toString());
		if(toolbar) {
			toolbar->setToolButtonStyle(Qt::ToolButtonIconOnly);
		}
	}
}

void MainWindow::setToolbarTextOnly()
{
	if(QAction *action = qobject_cast<QAction*>(sender())) {
		if(QToolBar *toolbar = findChild<QToolBar*>(action->property("toolbar").toString())) {
			toolbar->setToolButtonStyle(Qt::ToolButtonTextOnly);
		}
	}
}

void MainWindow::setToolbarTextBeside()
{
	if(QAction *action = qobject_cast<QAction*>(sender())) {
		if(QToolBar *toolbar = findChild<QToolBar*>(action->property("toolbar").toString())) {
			toolbar->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
		}
	}
}

void MainWindow::setToolbarTextUnder()
{
	if(QAction *action = qobject_cast<QAction*>(sender())) {
		if(QToolBar *toolbar = findChild<QToolBar*>(action->property("toolbar").toString())) {
			toolbar->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
		}
	}
}

void MainWindow::setFilterText(const QString &text)
{
	m_proxy->setFilterWildcard(text);
}

void MainWindow::toggleToolbarLock()
{
	if(QAction *action = qobject_cast<QAction*>(sender())) {
		if(QToolBar *toolbar = findChild<QToolBar*>(action->property("toolbar").toString())) {
			toolbar->setMovable(!toolbar->isMovable());
		}
	}
}

void MainWindow::changeEvent(QEvent *event)
{
	if(event->type() == QEvent::LanguageChange) {
		m_ui->retranslateUi(this);
		m_header->retranslate();
		ActionManager::inst()->retranslate();
	}

	QMainWindow::changeEvent(event);
}

void MainWindow::enterEvent(QEvent *event)
{
	QMainWindow::enterEvent(event);

	m_hideTimer.stop();
}

void MainWindow::leaveEvent(QEvent *event)
{
	QMainWindow::leaveEvent(event);

	if(Core::inst()->setting(KittySDK::Settings::S_MAINWINDOW_AUTOHIDE, false).toBool()) {
		m_hideTimer.start();
	}
}

void MainWindow::addContact(KittySDK::IContact *contact)
{
	RosterContact *cnt = new RosterContact(contact, m_model->groupItem(contact->group()));

	m_model->addContact(cnt, m_model->groupItem(contact->group()));
	m_proxy->invalidate();
}

bool MainWindow::eventFilter(QObject *obj, QEvent *event)
{
	if(event->type() == QEvent::KeyPress) {
		if(QKeyEvent *ev = static_cast<QKeyEvent*>(event)) {
			if(obj == m_ui->rosterTreeView) {
				if(!ev->text().isEmpty() && ev->text().at(0).isPrint()) {
					m_ui->filterEdit->show();
					m_ui->filterEdit->setText(ev->text());
					m_ui->filterEdit->setFocus();
				}
			} else if(obj == m_ui->filterEdit) {
				if(ev->key() == Qt::Key_Escape) {
					m_ui->filterEdit->clear();
					m_ui->filterEdit->hide();
					m_ui->rosterTreeView->setFocus();
				} else if((ev->key() == Qt::Key_Down) || (ev->key() == Qt::Key_Up)) {
					qApp->postEvent(m_ui->rosterTreeView, new QKeyEvent(QEvent::KeyPress, ev->key(), Qt::NoModifier));
				}
			}
		}
	}

	return QMainWindow::eventFilter(obj, event);
}

void MainWindow::requestHistory(KittySDK::IContact *contact)
{
	Core::inst()->historyWindow()->showContact(contact);
}

}
