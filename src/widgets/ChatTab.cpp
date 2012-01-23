#include "ChatTab.h"
#include "ui_ChatTab.h"

#include "widgets/windows/HistoryWindow.h"
#include "widgets/windows/MainWindow.h"
#include "widgets/windows/ChatWindow.h"
#include "widgets/ChatColorPicker.h"
#include "PluginManager.h"
#include "SDK/constants.h"
#include "SDK/Protocol.h"
#include "SDK/Contact.h"
#include "SDK/Message.h"
#include "IconManager.h"
#include "ChatTheme.h"
#include "SDK/Chat.h"
#include "Profile.h"
#include "Core.h"

#ifdef Q_WS_WIN32
#include <qt_windows.h>
#endif

#include <QtCore/QFile>
#include <QtGui/QToolButton>
#include <QtGui/QToolBar>
#include <QtGui/QMenu>
#include <QtWebKit/QWebElement>
#include <QtWebKit/QWebFrame>
#include <QtWebKit/QWebPage>

#define qDebug() qDebug() << "[ChatTab]"
#define qWarning() qWarning() << "[ChatTab]"

using namespace Kitty;
using namespace KittySDK;

Kitty::ChatTab::ChatTab(Chat *chat, QWidget *parent): QWidget(parent), m_ui(new Ui::ChatTab), m_chat(chat)
{
	m_ui->setupUi(this);

	connect(m_ui->textEdit, SIGNAL(returnPressed()), this, SLOT(sendMessage()));
	connect(m_ui->textEdit, SIGNAL(cursorPositionChanged()), this, SLOT(updateButtons()));
	connect(m_ui->webView, SIGNAL(keyPressed()), m_ui->textEdit, SLOT(setFocus()));
	connect(chat->contacts().first(), SIGNAL(statusChanged(KittySDK::Protocol::Status,QString)), this, SLOT(changeStatus(KittySDK::Protocol::Status, QString)));
	connect(chat->contacts().first(), SIGNAL(dataChanged()), this, SIGNAL(tabChanged()));
	connect(&m_cleanTimer, SIGNAL(timeout()), this, SLOT(clearMessages()));

	m_toolBar = new QToolBar(this);
	m_toolBar->setIconSize(QSize(16, 16));
	m_toolBar->setStyleSheet("QToolBar { border: 0; }");
	m_ui->verticalLayout->addWidget(m_toolBar);

	m_colorPicker = new ChatColorPicker(this);
	connect(m_colorPicker, SIGNAL(colorSelected(QColor)), m_ui->textEdit, SLOT(colorText(QColor)));

	m_messageCount = 0;

	if(chat->contacts().count() == 1) {
		m_ui->treeWidget->hide();
	} else {
		foreach(Contact *cnt, chat->contacts()) {
			QTreeWidgetItem *item = new QTreeWidgetItem(m_ui->treeWidget);
			item->setIcon(0, QIcon(Core::inst()->avatarPath(cnt)));
			item->setText(0, cnt->display());
		}
	}

	Protocol *proto = chat->account()->protocol();

	if(proto->abilities().testFlag(Protocol::TextBold)) {
		m_boldAction = m_toolBar->addAction(tr("Bold"), m_ui->textEdit, SLOT(boldText()));
		m_boldAction->setCheckable(true);
		m_boldAction->setProperty("icon_id", Icons::I_BOLD);
	}

	if(proto->abilities().testFlag(Protocol::TextItalics)) {
		m_italicAction = m_toolBar->addAction(tr("Italic"), m_ui->textEdit, SLOT(italicText()));
		m_italicAction->setCheckable(true);
		m_italicAction->setProperty("icon_id", Icons::I_ITALIC);
	}

	if(proto->abilities().testFlag(Protocol::TextUnderline)) {
		m_underlineAction = m_toolBar->addAction(tr("Underline"), m_ui->textEdit, SLOT(underlineText()));
		m_underlineAction->setCheckable(true);
		m_underlineAction->setProperty("icon_id", Icons::I_UNDERLINE);
	}

	if(proto->abilities().testFlag(Protocol::TextStrikethrough)) {
		m_strikethroughAction = m_toolBar->addAction(tr("Strikethrough"));
		m_strikethroughAction->setCheckable(true);
		m_strikethroughAction->setProperty("icon_id", Icons::I_STRIKETHROUGH);
	}

	if(m_toolBar->actions().count() > 0) {
		m_toolBar->addSeparator();
	}

	if(proto->abilities().testFlag(Protocol::TextColor)) {
		m_colorAction = m_toolBar->addAction(tr("Color"), this, SLOT(showColorPicker()));
		m_colorAction->setProperty("icon_id", Icons::I_COLOR);
	}

	m_smiliesAction = m_toolBar->addAction(tr("Smilies"));
	m_smiliesAction->setProperty("icon_id", Icons::I_SMILEY);

	m_toolBar->addSeparator();

	int c = m_toolBar->actions().count();

	if(proto->abilities().testFlag(Protocol::SendImages)) {
		m_imageAction = m_toolBar->addAction(tr("Send image"));
		m_imageAction->setProperty("icon_id", Icons::I_IMAGE);

		QMenu *imageMenu = new QMenu(this);
		imageMenu->addAction(tr("From file..."));
		imageMenu->addAction(tr("Desktop snapshot"));
		imageMenu->addAction(tr("Desktop snapshot fragment"));

#ifdef Q_WS_WIN32
		QAction *windowAction = imageMenu->addAction(tr("Window snapshot"));

		QMenu *windowMenu = new QMenu(this);

		QList<HWND> visited;

		HWND hWnd = GetWindow(GetDesktopWindow(), GW_CHILD);
		while(hWnd) {
			if(visited.contains(hWnd)) {
				break;
			}

			visited.append(hWnd);

			if((GetWindowTextLength(hWnd) > 0)) {
				LONG style = GetWindowLong(hWnd, GWL_STYLE);

				if((style & WS_VISIBLE) && (style & WS_CAPTION)) {
					WCHAR *text = new WCHAR[255];
					GetWindowText(hWnd, text, 250);

					QAction *action = windowMenu->addAction(QString::fromWCharArray(text));
					//QPixmap::grabWindow(hWnd).save(QString("C:/%1.png").arg((int)hWnd));

					HICON hIcon = (HICON)SendMessage(hWnd, WM_GETICON, ICON_SMALL, NULL);
					if(!hIcon) {
						hIcon = (HICON)SendMessage(hWnd, WM_GETICON, 2, NULL);
					}

					if(!hIcon) {
						hIcon = (HICON)SendMessage(hWnd, WM_GETICON, ICON_BIG, NULL);
					}

					if(!hIcon) {
						hIcon = (HICON)GetClassLongPtr(hWnd, GCL_HICON);
					}

					if(!hIcon) {
						hIcon = (HICON)GetClassLongPtr(hWnd, GCL_HICONSM);
					}

					if(hIcon) {
						QPixmap icon = QPixmap::fromWinHICON(hIcon);
						action->setIcon(icon);
					}

					delete text;
					delete hIcon;
				}
			}

			hWnd = GetWindow(hWnd, GW_HWNDNEXT);
		}

		windowAction->setMenu(windowMenu);
#endif

		imageMenu->addAction(tr("Clipboard contents"));

		m_imageAction->setMenu(imageMenu);

		if(QToolButton *imageButton = qobject_cast<QToolButton*>(m_toolBar->widgetForAction(m_imageAction))) {
			imageButton->setPopupMode(QToolButton::InstantPopup);
		}
	}

	if(proto->abilities().testFlag(Protocol::SendFiles)) {
		m_fileAction = m_toolBar->addAction(tr("Send file"));
		m_fileAction->setProperty("icon_id", Icons::I_FILE);
	}

	if(m_toolBar->actions().count() > c) {
		m_toolBar->addSeparator();
	}

	m_profileAction = m_toolBar->addAction(tr("vCard"), this, SLOT(showContactWindow()));
	m_profileAction->setProperty("icon_id", Icons::I_PROFILE);

	m_historyAction = m_toolBar->addAction(tr("History"), this, SLOT(showHistoryWindow()));
	m_historyAction->setProperty("icon_id", Icons::I_HISTORY);

	updateIcons();

	m_ui->webView->clear();

	applySettings();
}

Kitty::ChatTab::~ChatTab()
{
	delete m_boldAction;
	delete m_italicAction;
	delete m_underlineAction;
	delete m_strikethroughAction;
	delete m_colorAction;
	delete m_smiliesAction;
	delete m_imageAction;
	delete m_fileAction;
	delete m_profileAction;
	delete m_historyAction;

	delete m_ui;
}

void Kitty::ChatTab::updateIcons()
{
	Core *core = Core::inst();

	foreach(QAction *action, m_toolBar->actions()) {
		if(action->property("icon_id").isValid()) {
			action->setIcon(core->icon(action->property("icon_id").toString()));
		}
	}
}

void Kitty::ChatTab::setEditFocus()
{
	m_ui->textEdit->setFocus();
}

void Kitty::ChatTab::applySettings()
{
	Core *core = Core::inst();

	m_ui->textEdit->clearHistory();

	if(core->setting(Settings::S_CHATWINDOW_CLEAR_MESSAGES).toInt() == 0) {
		m_messageCount = 0;
	}

	if(core->setting(Settings::S_CHATWINDOW_CLEAR_INTERVAL).toInt() == 0) {
		m_cleanTimer.stop();
	} else {
		m_cleanTimer.setInterval(core->setting(Settings::S_CHATWINDOW_CLEAR_INTERVAL).toInt() * 60 * 1000);
		m_cleanTimer.start();
	}
}

void Kitty::ChatTab::appendMessage(KittySDK::Message &msg)
{
	Core *core = Core::inst();
	if(core->setting(Settings::S_CHATWINDOW_CLEAR_MESSAGES).toInt() > 0) {
		if(m_messageCount >= core->setting(Settings::S_CHATWINDOW_CLEAR_MESSAGES).toInt()) {
			clearMessages();
		}

		m_messageCount++;
	}

	core->archiveMessage(msg);

	m_ui->webView->appendMessage(msg);
}

void Kitty::ChatTab::clearMessages()
{
	m_messageCount = 0;
	m_ui->webView->clear();
}

void Kitty::ChatTab::sendMessage()
{
	if(!m_ui->textEdit->toPlainText().isEmpty()) {
		QString html =  m_ui->textEdit->toHtml();

		QRegExp regexp;
		regexp.setMinimal(true);

		regexp.setPattern("<p.*>(.*)</p>");
		html.replace(regexp, "\\1");

		regexp.setPattern("<!DOCTYPE.*<body.*>");
		html.remove(regexp);

		regexp.setPattern("</body.*");
		regexp.setMinimal(false);
		html.remove(regexp);

		Message msg(m_chat->me(), m_chat->contacts());
		msg.setBody(html.trimmed());
		msg.setChat(m_chat);

		QList<Plugin*> plugins = PluginManager::inst()->plugins();
		foreach(Plugin *plugin, plugins) {
			if(plugin->isLoaded()) {
				plugin->plugin()->processMessage(msg);
			}
		}

		appendMessage(msg);

		if(Core::inst()->setting(Settings::S_CHATWINDOW_SENTHISTORY, true).toBool()) {
			m_ui->textEdit->addHistory(msg.body());
		}

		m_chat->account()->sendMessage(msg);
		m_ui->textEdit->clear();
	}
}

void Kitty::ChatTab::changeStatus(KittySDK::Protocol::Status status, QString description)
{
	emit tabChanged();
}

void Kitty::ChatTab::changeEvent(QEvent *event)
{
	if(event->type() == QEvent::LanguageChange) {
		m_ui->retranslateUi(this);

		if(m_boldAction) {
			m_boldAction->setText(tr("Bold"));
		}

		if(m_italicAction) {
			m_italicAction->setText(tr("Italic"));
		}

		if(m_underlineAction) {
			m_underlineAction->setText(tr("Underline"));
		}

		if(m_strikethroughAction) {
			m_strikethroughAction->setText(tr("Striketrough"));
		}

		if(m_colorAction) {
			m_colorAction->setText(tr("Color"));
		}

		if(m_smiliesAction) {
			m_smiliesAction->setText(tr("Smilies"));
		}

		if(m_imageAction) {
			m_imageAction->setText(tr("Send image"));
		}

		if(m_fileAction) {
			m_fileAction->setText(tr("Send file"));
		}

		if(m_profileAction) {
			m_profileAction->setText(tr("Profile"));
		}

		if(m_historyAction) {
			m_historyAction->setText(tr("History"));
		}

		m_colorPicker->retranslate();
	}

	QWidget::changeEvent(event);
}

void Kitty::ChatTab::updateButtons()
{
	QTextCharFormat fmt = m_ui->textEdit->currentCharFormat();

	if(m_boldAction) {
		m_boldAction->setChecked(fmt.fontWeight() == QFont::Bold);
	}

	if(m_italicAction) {
		m_italicAction->setChecked(fmt.fontItalic());
	}

	if(m_underlineAction) {
		m_underlineAction->setChecked(fmt.fontUnderline());
	}

	if(m_strikethroughAction) {
		m_strikethroughAction->setChecked(fmt.fontStrikeOut());
	}
}

void Kitty::ChatTab::showColorPicker()
{
	if(QAction *action = qobject_cast<QAction*>(sender())) {
		if(QWidget *widget = m_toolBar->widgetForAction(action)) {
			m_colorPicker->showAt(widget->mapToGlobal(QPoint(0, widget->height())));
		}
	}
}

void Kitty::ChatTab::showContactWindow()
{
	Core::inst()->showContactWindow(m_chat->contacts().first());
}

void Kitty::ChatTab::showHistoryWindow()
{
	Core::inst()->historyWindow()->showContact(m_chat->contacts().first());
}
