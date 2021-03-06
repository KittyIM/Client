#include "ChatWindow.h"
#include "ui_ChatWindow.h"

#include "widgets/windows/SettingsWindow.h"
#include "3rdparty/qtwin/qtwin.h"
#include "widgets/ChatTab.h"
#include "EmoticonManager.h"
#include "IconManager.h"
#include "ChatTheme.h"
#include "Core.h"

#include <SDKConstants.h>
#include <IContact.h>
#include <IChat.h>

#include <QtCore/QDebug>
#include <QtGui/QKeyEvent>

#define qDebug() qDebug() << "ChatWindow"
#define qWarning() qWarning() << "ChatWindow"

namespace Kitty
{

ChatWindow::ChatWindow(Core *core, QWidget *parent):
	QWidget(parent),
	m_ui(new Ui::ChatWindow),
	m_core(core)
{
	m_ui->setupUi(this);

	connect(m_core->iconManager(), SIGNAL(iconsUpdated()), m_ui->tabWidget, SLOT(updateIcons()));
	connect(m_core, SIGNAL(settingsApplied()), SLOT(applySettings()));
	connect(m_core, SIGNAL(settingsApplied()), m_ui->tabWidget, SLOT(applySettings()));

	qDebug() << "Creating";

	restoreGeometry(m_core->setting(KittySDK::Settings::S_CHATWINDOW_GEOMETRY).toByteArray());

	m_theme = 0;

	applySettings();

	if(QtWin::isCompositionEnabled()) {
		QtWin::extendFrameIntoClientArea(this);
		setContentsMargins(0, 0, 0, 0);
	}
}

ChatWindow::~ChatWindow()
{
	m_core->setSetting(KittySDK::Settings::S_CHATWINDOW_GEOMETRY, saveGeometry());

	delete m_ui;
}

ChatTab *ChatWindow::tabByChat(KittySDK::IChat *chat)
{
	return m_ui->tabWidget->tabByChat(chat);
}

bool ChatWindow::isChatActive(KittySDK::IChat *chat)
{
	return (m_ui->tabWidget->currentIndex() == m_ui->tabWidget->indexByChat(chat));
}

void ChatWindow::applySettings()
{
	m_core->emoticonManager()->load();

	if(m_theme) {
		delete m_theme;
	}

	m_theme = new ChatTheme(m_core->setting(KittySDK::Settings::S_CHAT_THEME).toString(), this);

	on_tabWidget_currentChanged(m_ui->tabWidget->currentIndex());
}

ChatTab *ChatWindow::startChat(KittySDK::IChat *chat)
{
	return m_ui->tabWidget->startChat(chat);
}

void ChatWindow::switchTo(KittySDK::IChat *chat)
{
	m_ui->tabWidget->switchTo(chat);
}

void ChatWindow::showChat(KittySDK::IChat *chat)
{
	startChat(chat);
	switchTo(chat);

	show();
	activateWindow();
	setWindowState(windowState() & (~Qt::WindowMinimized | Qt::WindowActive));
}

void ChatWindow::on_tabWidget_tabCloseRequested(int index)
{
	m_ui->tabWidget->removeTab(index);

	if(m_ui->tabWidget->count() == 0) {
		close();
	}
}

void ChatWindow::keyPressEvent(QKeyEvent *event)
{
	if(m_core->setting(KittySDK::Settings::S_CHATWINDOW_TABBAR_FKEYS, false).toBool()) {
		if((event->key() >= Qt::Key_F1) && (event->key() <= Qt::Key_F12)) {
			m_ui->tabWidget->setCurrentIndex(event->key() - (int)Qt::Key_F1);
		}
	}

	QWidget::keyPressEvent(event);
}

void ChatWindow::closeEvent(QCloseEvent *event)
{
	//Close all tabs
	if(m_core->setting(KittySDK::Settings::S_CHATWINDOW_TABBAR_CLOSE_WND, false).toBool()) {
		m_ui->tabWidget->clear();
	}

	QWidget::closeEvent(event);
}

void ChatWindow::on_tabWidget_currentChanged(int index)
{
	if(ChatTab *tab = qobject_cast<ChatTab*>(m_ui->tabWidget->widget(index))) {
		if(tab->chat()->contacts().count() == 1) {
			if(KittySDK::IContact *cnt = tab->chat()->contacts().first()) {
				QString title = m_core->setting(KittySDK::Settings::S_CHATWINDOW_CAPTION, "%display% [%status%] \"%description%\"").toString();
				title.replace("%display%", cnt->display());
				title.replace("%status%", m_core->statusToString(cnt->status()));

				if(cnt->description().length() > 0) {
					title.replace("%description%", QString("\"%1\"").arg(cnt->description()));
				} else {
					title.replace("%description%", "");
				}

				title.replace("%uid%", cnt->uid());
				title.replace("%nickname%", cnt->data(KittySDK::ContactInfos::I_NICKNAME).toString());
				title.replace("%firstname%", cnt->data(KittySDK::ContactInfos::I_FIRSTNAME).toString());
				title.replace("%lastname%", cnt->data(KittySDK::ContactInfos::I_LASTNAME).toString());

				int sex = cnt->data(KittySDK::ContactInfos::I_SEX).toInt();
				if(sex == 0) {
					title.replace("%sex%", tr("Unknown"));
				} else if(sex == 1) {
					title.replace("%sex%", tr("Female"));
				} else {
					title.replace("%sex%", tr("Male"));
				}

				//title.replace("%phone%", cnt->data(KittySDK::ContactInfos::I_NICKNAME));
				//title.replace("%email%", cnt->data(KittySDK::ContactInfos::I_NICKNAME));

				setWindowTitle(title);
			}
		} else {
			QString title;
			foreach(KittySDK::IContact *cnt, tab->chat()->contacts()) {
				title += cnt->display() + ", ";
			}

			title.chop(2);
			setWindowTitle(title);
		}
	}
}

}
