#include "ChatTab.h"
#include "ui_ChatTab.h"

#include "widgets/DesktopFragmentDialog.h"
#include "widgets/windows/HistoryWindow.h"
#include "widgets/windows/MainWindow.h"
#include "widgets/windows/ChatWindow.h"
#include "widgets/ChatColorPicker.h"
#include "PluginManager.h"
#include "MessageQueue.h"
#include "IconManager.h"
#include "ChatTheme.h"
#include "Profile.h"
#include "Core.h"

#include <SDKConstants.h>
#include <IProtocol.h>
#include <IContact.h>
#include <IMessage.h>
#include <IChat.h>

#ifdef Q_WS_WIN32
#define _WIN32_WINNT 0x0501
#include <qt_windows.h>
#endif

#include <QtCore/QMimeData>
#include <QtCore/QFile>
#include <QtGui/QDesktopWidget>
#include <QtGui/QFileDialog>
#include <QtGui/QToolButton>
#include <QtGui/QClipboard>
#include <QtGui/QKeyEvent>
#include <QtGui/QToolBar>
#include <QtGui/QLabel>
#include <QtGui/QMenu>
#include <QtWebKit/QWebElement>
#include <QtWebKit/QWebFrame>
#include <QtWebKit/QWebPage>

#define qDebug() qDebug() << "[ChatTab]"
#define qWarning() qWarning() << "[ChatTab]"

namespace Kitty
{

ChatTab::ChatTab(KittySDK::IChat *chat, QWidget *parent):
	QWidget(parent),
	m_ui(new Ui::ChatTab),
	m_chat(chat)
{
	m_ui->setupUi(this);

	connect(m_ui->textEdit, SIGNAL(returnPressed()), SLOT(sendMessage()));
	connect(m_ui->textEdit, SIGNAL(currentCharFormatChanged(QTextCharFormat)), SLOT(updateButtons(QTextCharFormat)));
	connect(m_ui->textEdit, SIGNAL(typingChanged(bool,int)), SLOT(sendTypingNotify(bool,int)));
	connect(m_ui->textEdit, SIGNAL(pixmapDropped(QPixmap)), SLOT(sendPixmap(QPixmap)));
	connect(m_ui->webView, SIGNAL(keyPressed()), m_ui->textEdit, SLOT(setFocus()));
	connect(chat->contacts().first(), SIGNAL(statusChanged(KittySDK::IProtocol::Status,QString)), SLOT(changeStatus(KittySDK::IProtocol::Status, QString)));
	connect(chat->contacts().first(), SIGNAL(dataChanged()), SIGNAL(tabChanged()));
	connect(&m_cleanTimer, SIGNAL(timeout()), SLOT(clearMessages()));
	connect(Core::inst()->messageQueue(), SIGNAL(messageEnqueued(quint32,KittySDK::IMessage)), SLOT(checkDequeue(quint32,KittySDK::IMessage)));
	connect(m_ui->textEdit, SIGNAL(focusedIn()), SLOT(editFocused()));

	m_toolBar = new QToolBar(this);
	m_toolBar->setIconSize(QSize(16, 16));
	m_toolBar->setMaximumHeight(30);
	m_toolBar->setStyleSheet("QToolBar { border: 0; }");

	m_colorPicker = new ChatColorPicker(this);
	connect(m_colorPicker, SIGNAL(colorSelected(QColor)), m_ui->textEdit, SLOT(colorText(QColor)));

	m_messageCount = 0;

	if(chat->contacts().count() == 1) {
		m_ui->treeWidget->hide();
	} else {
		foreach(KittySDK::IContact *cnt, chat->contacts()) {
			QTreeWidgetItem *item = new QTreeWidgetItem(m_ui->treeWidget);
			item->setIcon(0, QIcon(Core::inst()->avatarPath(cnt)));
			item->setText(0, cnt->display());
		}
	}

	KittySDK::IProtocol *proto = chat->account()->protocol();

	if(proto->abilities() & KittySDK::IProtocol::TextBold) {
		m_boldAction = m_toolBar->addAction(tr("Bold"), m_ui->textEdit, SLOT(boldText()));
		m_boldAction->setCheckable(true);
		m_boldAction->setShortcut(QKeySequence::Bold);
		m_boldAction->setProperty("icon_id", KittySDK::Icons::I_BOLD);
	}

	if(proto->abilities() & KittySDK::IProtocol::TextItalics) {
		m_italicAction = m_toolBar->addAction(tr("Italic"), m_ui->textEdit, SLOT(italicText()));
		m_italicAction->setCheckable(true);
		m_italicAction->setShortcut(QKeySequence::Italic);
		m_italicAction->setProperty("icon_id", KittySDK::Icons::I_ITALIC);
	}

	if(proto->abilities() & KittySDK::IProtocol::TextUnderline) {
		m_underlineAction = m_toolBar->addAction(tr("Underline"), m_ui->textEdit, SLOT(underlineText()));
		m_underlineAction->setCheckable(true);
		m_underlineAction->setShortcut(QKeySequence::Underline);
		m_underlineAction->setProperty("icon_id", KittySDK::Icons::I_UNDERLINE);
	}

	if(proto->abilities() & KittySDK::IProtocol::TextStrikethrough) {
		m_strikethroughAction = m_toolBar->addAction(tr("Strikethrough"));
		m_strikethroughAction->setCheckable(true);
		m_strikethroughAction->setProperty("icon_id", KittySDK::Icons::I_STRIKETHROUGH);
	}

	if(m_toolBar->actions().count() > 0) {
		m_toolBar->addSeparator();
	}

	if(proto->abilities() & KittySDK::IProtocol::TextColor) {
		m_colorAction = m_toolBar->addAction(tr("Color"), this, SLOT(showColorPicker()));
		m_colorAction->setProperty("icon_id", KittySDK::Icons::I_COLOR);
	}

	m_smiliesAction = m_toolBar->addAction(tr("Smilies"));
	m_smiliesAction->setProperty("icon_id", KittySDK::Icons::I_SMILEY);

	m_toolBar->addSeparator();

	int old_count = m_toolBar->actions().count();

	if(proto->abilities() & KittySDK::IProtocol::SendImages) {
		m_imageAction = m_toolBar->addAction(tr("Send image"));
		m_imageAction->setProperty("icon_id", KittySDK::Icons::I_IMAGE);

		QMenu *imageMenu = new QMenu(this);
		connect(imageMenu, SIGNAL(aboutToShow()), SLOT(updateImageMenu()));
		imageMenu->addAction(tr("From file..."), this, SLOT(sendImageFile()));

		QAction *desktopAction = imageMenu->addAction(tr("Desktop snapshot"));
		if(QApplication::desktop()->screenCount() > 1) {
			QMenu *desktopMenu = new QMenu(this);

			for(int i = 0; i < QApplication::desktop()->screenCount(); ++i) {
				QAction *action = desktopMenu->addAction(tr("Desktop") + " #" + QString::number(i + 1), this, SLOT(sendImageDesktop()));
				action->setData(i);
			}

			desktopAction->setMenu(desktopMenu);
		} else {
			desktopAction->setData(0);
			connect(desktopAction, SIGNAL(triggered()), SLOT(sendImageDesktop()));
		}

		imageMenu->addAction(tr("Desktop snapshot fragment"), this, SLOT(sendImageFragment()));

#ifdef Q_WS_WIN32
		QAction *windowAction = imageMenu->addAction(tr("Window snapshot"));
		windowAction->setMenu(new QMenu(this));
#endif

		imageMenu->addAction(tr("Clipboard contents"), this, SLOT(sendImageClipboard()));

		m_imageAction->setMenu(imageMenu);

		if(QToolButton *imageButton = qobject_cast<QToolButton*>(m_toolBar->widgetForAction(m_imageAction))) {
			imageButton->setPopupMode(QToolButton::InstantPopup);
		}
	}

	if(proto->abilities() & KittySDK::IProtocol::SendFiles) {
		m_fileAction = m_toolBar->addAction(tr("Send file"));
		m_fileAction->setProperty("icon_id", KittySDK::Icons::I_FILE);
	}

	if(m_toolBar->actions().count() != old_count) {
		m_toolBar->addSeparator();
	}

	m_profileAction = m_toolBar->addAction(tr("vCard"), this, SLOT(showContactWindow()));
	m_profileAction->setProperty("icon_id", KittySDK::Icons::I_PROFILE);

	m_historyAction = m_toolBar->addAction(tr("History"), this, SLOT(showHistoryWindow()));
	m_historyAction->setProperty("icon_id", KittySDK::Icons::I_HISTORY);

	if(proto->abilities() & KittySDK::IProtocol::TypingNotification) {
		m_toolBar->addSeparator();

		m_typingLabel = new QLabel(this);
		m_typingLabel->setToolTip(tr("Contact is not typing"));
		m_typingLabel->setPixmap(Core::inst()->icon(KittySDK::Icons::I_TYPING_OFF));
		m_toolBar->addWidget(m_typingLabel);
	}

	updateIcons();

	m_ui->webView->clear();

	applySettings();
}

ChatTab::~ChatTab()
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

void ChatTab::setTypingNotify(bool typing, const int &length)
{
	if(m_typingLabel) {
		if(typing) {
			QPixmap icon(Core::inst()->icon(KittySDK::Icons::I_TYPING_ON));

			//draw length on the icon
			if(length) {
				QPainter p(&icon);
				QFont font = p.font();
				font.setPixelSize(icon.height() * 0.5);
				p.setFont(font);

				int width = p.fontMetrics().width(QString::number(length));
				while(width >= icon.width()) {
					font.setPixelSize(font.pixelSize() - 1);
					p.setFont(font);

					width = p.fontMetrics().width(QString::number(length));
				}

				p.setBrush(QColor(255, 255, 255, 128));
				p.setPen(QPen(Qt::black, 1.0));

				QRect rect(icon.width() - width - 4 * p.pen().width(), icon.height() * 0.45, width + 4 * p.pen().width(), icon.height() * 0.55);

				p.fillRect(rect, QColor(255, 255, 255, 128));

				p.drawText(rect, QString::number(length), QTextOption(Qt::AlignCenter));
				p.end();
			}

			m_typingLabel->setToolTip(tr("Contact is typing"));
			m_typingLabel->setPixmap(icon);
		} else {
			m_typingLabel->setToolTip(tr("Contact is not typing"));
			m_typingLabel->setPixmap(Core::inst()->icon(KittySDK::Icons::I_TYPING_OFF));
		}

		m_typingLabel->setProperty("typing", typing);
		m_typingLabel->setProperty("length", length);
	}
}

void ChatTab::updateIcons()
{
	Core *core = Core::inst();

	foreach(QAction *action, m_toolBar->actions()) {
		if(action->property("icon_id").isValid()) {
			action->setIcon(core->icon(action->property("icon_id").toString()));
		}
	}
}

void ChatTab::setEditFocus()
{
	Core::inst()->messageQueue()->dequeue(m_chat);
	m_ui->textEdit->setFocus();
}

void ChatTab::applySettings()
{
	Core *core = Core::inst();

	m_ui->textEdit->clearHistory();

	if(core->setting(KittySDK::Settings::S_CHATWINDOW_CLEAR_MESSAGES).toInt() == 0) {
		m_messageCount = 0;
	}

	if(core->setting(KittySDK::Settings::S_CHATWINDOW_CLEAR_INTERVAL).toInt() == 0) {
		m_cleanTimer.stop();
	} else {
		m_cleanTimer.setInterval(core->setting(KittySDK::Settings::S_CHATWINDOW_CLEAR_INTERVAL).toInt() * 60 * 1000);
		m_cleanTimer.start();
	}

	m_ui->verticalLayout->removeWidget(m_toolBar);
	if(core->setting(KittySDK::Settings::S_CHATWINDOW_TOOLBAR_POS, 1).toInt() == 0) {
		m_ui->verticalLayout->insertWidget(1, m_toolBar);
	} else {
		m_ui->verticalLayout->insertWidget(2, m_toolBar);
	}

	QFont previous_font = m_ui->textEdit->font();

	QFont font = qApp->font();
	font.setFamily(core->setting(KittySDK::Settings::S_CHATEDIT_FONT_FAMILY, qApp->font().family()).toString());
	font.setPointSize(core->setting(KittySDK::Settings::S_CHATEDIT_FONT_SIZE, qApp->font().pointSize()).toInt());
	font.setStyleStrategy(core->setting(KittySDK::Settings::S_CHATEDIT_FONT_ANTIALIASING, false).toBool() ? QFont::PreferAntialias : QFont::NoAntialias);

	if(font != previous_font) {
		QString text = m_ui->textEdit->toPlainText();

		m_ui->textEdit->setFont(font);
		m_ui->textEdit->document()->setDefaultFont(font);
		m_ui->textEdit->document()->clear();
		m_ui->textEdit->resetCharFormat();

		m_ui->textEdit->setPlainText(text);
	}

	m_ui->textEdit->updateSize();
}

void ChatTab::appendMessage(KittySDK::IMessage &msg)
{
	Core *core = Core::inst();
	if(core->setting(KittySDK::Settings::S_CHATWINDOW_CLEAR_MESSAGES).toInt() > 0) {
		if(m_messageCount >= core->setting(KittySDK::Settings::S_CHATWINDOW_CLEAR_MESSAGES).toInt()) {
			clearMessages();
		}

		++m_messageCount;
	}

	core->archiveMessage(msg);

	m_ui->webView->appendMessage(msg);
}

void ChatTab::clearMessages()
{
	m_messageCount = 0;
	m_ui->webView->clear();
}

void ChatTab::sendMessage()
{
	if(!m_ui->textEdit->toPlainText().isEmpty()) {
		QString html = m_ui->textEdit->toHtml();

		QRegExp regexp;
		regexp.setMinimal(true);

		regexp.setPattern("<p.*>(.*)</p>");
		html.replace(regexp, "\\1");

		regexp.setPattern("<!DOCTYPE.*<body.*>");
		html.remove(regexp);

		regexp.setPattern("</body.*");
		regexp.setMinimal(false);
		html.remove(regexp);

		KittySDK::IMessage msg(m_chat->me(), m_chat->contacts());
		msg.setBody(html.trimmed());
		msg.setChat(m_chat);

		QList<Plugin*> plugins = Core::inst()->pluginManager()->plugins();
		foreach(Plugin *plugin, plugins) {
			if(plugin->isLoaded()) {
				plugin->iplugin()->processMessage(msg);
			}
		}

		appendMessage(msg);

		if(Core::inst()->setting(KittySDK::Settings::S_CHATWINDOW_SENTHISTORY, true).toBool()) {
			m_ui->textEdit->addHistory(msg.body());
		}

		m_chat->account()->sendMessage(msg);

		m_ui->textEdit->clear();
		m_ui->textEdit->resetCharFormat();
	}
}

void ChatTab::sendTypingNotify(bool typing, const int &length)
{
	if(KittySDK::IContact *cnt = m_chat->contacts().first()) {
		m_chat->account()->sendTypingNotify(cnt, typing, length);
	}
}

void ChatTab::sendImageFile()
{
	QString fileName = QFileDialog::getOpenFileName(this, tr("Select file"), QString(), tr("Images") + " (*.jpg *.jpeg *.gif *.png *.bmp)");
	if(!fileName.isEmpty()) {
		QFileInfo fileInfo(fileName);

		QDir imgDir(Core::inst()->currentProfileDir() + "imgcache/");
		if(imgDir.exists(fileInfo.fileName())) {
			QFile newFile(fileName);
			QFile oldFile(imgDir.filePath(fileInfo.fileName()));

			//file exists, but are they the same?
			if(newFile.open(QFile::ReadOnly) && oldFile.open(QFile::ReadOnly)) {
				//same files, use the cached one
				fileName = imgDir.filePath(fileInfo.fileName());
			} else {
				//different files, find unique name, copy and use the new one
				QString newFileName;
				int i = 2;
				do {
					newFileName = imgDir.filePath(fileInfo.completeBaseName() + "_" + QString::number(i) + "." + fileInfo.suffix());
					++i;
				} while(QFile::exists(newFileName));

				QFile::copy(fileName, newFileName);
				fileName = newFileName;
			}

			newFile.close();
			oldFile.close();
		} else {
			//new file, copy to cache
			QFile::copy(fileName, imgDir.filePath(fileInfo.fileName()));
			fileName = imgDir.filePath(fileInfo.fileName());
		}

		sendImage(fileName);
	}
}

void ChatTab::sendImageWindow()
{
#ifdef Q_WS_WIN32
	if(QAction *action = qobject_cast<QAction*>(sender())) {
		HWND hWnd = reinterpret_cast<HWND>(action->data().toInt());

		bool minimized = (GetWindowLong(hWnd, GWL_STYLE) & WS_MINIMIZE);

		if(minimized) {
			ShowWindow(hWnd, SW_RESTORE);
		}

		HDC hDC = GetDC(hWnd);
		HDC hTargetDC = CreateCompatibleDC(hDC);
		RECT rect;

		GetWindowRect(hWnd, &rect);

		HBITMAP hBitmap = CreateCompatibleBitmap(hDC, rect.right - rect.left, rect.bottom - rect.top);
		SelectObject(hTargetDC, hBitmap);

		PrintWindow(hWnd, hTargetDC, 0);

		if(minimized) {
			ShowWindow(hWnd, SW_MINIMIZE);
		}

		QPixmap window = QPixmap::fromWinHBITMAP(hBitmap);

		DeleteObject(hBitmap);
		ReleaseDC(hWnd, hDC);
		DeleteDC(hTargetDC);

		sendPixmap(window);
	}
#endif
}

void ChatTab::sendImageDesktop()
{
	if(QAction *action = qobject_cast<QAction*>(sender())) {
		int screen = action->data().toInt();
		QRect screenRect = QApplication::desktop()->screenGeometry(screen);

		QPixmap desktop = QPixmap::grabWindow(QApplication::desktop()->winId(), screenRect.left(), screenRect.top(), screenRect.width(), screenRect.height());
		sendPixmap(desktop);
	}
}

void ChatTab::sendImageClipboard()
{
	const QMimeData *mimeData = QApplication::clipboard()->mimeData();
	if(mimeData->hasImage()) {
		QPixmap clipboard = qvariant_cast<QPixmap>(mimeData->imageData());
		sendPixmap(clipboard);
	}
}

void ChatTab::sendImageFragment()
{
	QPixmap fragment = DesktopFragmentDialog::getPixmap(this);
	sendPixmap(fragment);
}

void ChatTab::sendPixmap(const QPixmap &pix)
{
	if(!pix.isNull()) {
		QDir imgDir(Core::inst()->currentProfileDir() + "imgcache");

		imgDir.mkpath(".");

		QString fileName = imgDir.absolutePath() + "/screenshot" + QString::number(QDateTime::currentDateTime().toTime_t()) + ".png";
		pix.save(fileName);
		sendImage(fileName);
	}
}

void ChatTab::sendImage(const QString &fileName)
{
	KittySDK::IMessage msg(m_chat->me(), m_chat->contacts());
	msg.setBody(QString("<img src=\"%1\" alt=\"%2\" title=\"%2\">").arg(fileName).arg(Qt::escape(QFileInfo(fileName).fileName())));
	msg.setChat(m_chat);

	QList<Plugin*> plugins = Core::inst()->pluginManager()->plugins();
	foreach(Plugin *plugin, plugins) {
		if(plugin->isLoaded()) {
			plugin->iplugin()->processMessage(msg);
		}
	}

	appendMessage(msg);

	if(Core::inst()->setting(KittySDK::Settings::S_CHATWINDOW_SENTHISTORY, true).toBool()) {
		m_ui->textEdit->addHistory(msg.body());
	}

	m_chat->account()->sendMessage(msg);
}

void ChatTab::changeStatus(KittySDK::IProtocol::Status status, QString description)
{
	if(m_chat->contacts().count() == 1) {
		if(Core::inst()->setting(KittySDK::Settings::S_CHATWINDOW_STATUS_CHANGES, true).toBool()) {
			KittySDK::IContact *cnt = m_chat->contacts().first();

			KittySDK::IMessage msg(cnt, m_chat->me());
			msg.setDirection(KittySDK::IMessage::System);
			msg.setBody(cnt->display() + " " + tr("changes status to") + " " + Core::inst()->statusToString(status) + (description.length() ? " \"" + description + "\"" : ""));

			m_ui->webView->appendMessage(msg);
		}
	}

	emit tabChanged();
}

void ChatTab::changeEvent(QEvent *event)
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

		if(m_typingLabel) {
			setTypingNotify(m_typingLabel->property("typing").toBool(), m_typingLabel->property("length").toInt());
		}

		m_colorPicker->retranslate();
	}

	QWidget::changeEvent(event);
}

void ChatTab::showEvent(QShowEvent *event)
{
	QWidget::showEvent(event);

	if(!event->spontaneous()) {
		Core::inst()->messageQueue()->dequeue(m_chat);
	}
}

void ChatTab::updateButtons(const QTextCharFormat &format)
{
	QFont font = format.font();

	if(m_boldAction) {
		m_boldAction->setChecked(font.bold());
	}

	if(m_italicAction) {
		m_italicAction->setChecked(font.italic());
	}

	if(m_underlineAction) {
		m_underlineAction->setChecked(font.underline());
	}

	if(m_strikethroughAction) {
		m_strikethroughAction->setChecked(font.strikeOut());
	}
}

void ChatTab::updateImageMenu()
{
	QMenu *imageMenu = m_imageAction->menu();
	if(imageMenu) {
		QAction *clipboardAction = imageMenu->actions().last();
		if(clipboardAction) {
			clipboardAction->setEnabled(QApplication::clipboard()->mimeData()->hasImage());
		}

#ifdef Q_WS_WIN32
		QAction *windowAction = imageMenu->actions().at(3);
		if(windowAction) {
			QMenu *windowMenu = windowAction->menu();
			if(windowMenu) {
				windowMenu->clear();

				QList<HWND> visited;

				HWND hWnd = GetWindow(GetDesktopWindow(), GW_CHILD);
				while(hWnd) {
					if(visited.contains(hWnd)) {
						break;
					}

					visited << hWnd;

					RECT rect;
					if(GetWindowRect(hWnd, &rect) && (rect.right - rect.left > 20) && (rect.bottom - rect.top > 20)) {
						LONG style = GetWindowLong(hWnd, GWL_STYLE);
						if((style & WS_VISIBLE) && (style & WS_CAPTION)) {
							WCHAR *text = new WCHAR[255];
							GetWindowText(hWnd, text, 250);

							const int maxTitleLength = 60;

							QString title = QString::fromWCharArray(text);
							if(title.length() > maxTitleLength) {
								title = title.left(maxTitleLength) + "...";
							}

							QAction *action = windowMenu->addAction(title, this, SLOT(sendImageWindow()));
							action->setData((int)hWnd);

							HICON hIcon = (HICON)SendMessage(hWnd, WM_GETICON, ICON_SMALL, 0);
							if(!hIcon) {
								hIcon = (HICON)SendMessage(hWnd, WM_GETICON, 2, 0);
							}

							if(!hIcon) {
								hIcon = (HICON)SendMessage(hWnd, WM_GETICON, ICON_BIG, 0);
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

							delete [] text;
							delete hIcon;
						}
					}

					hWnd = GetWindow(hWnd, GW_HWNDNEXT);
				}
			}
		}
#endif
	}
}

void ChatTab::showColorPicker()
{
	if(QAction *action = qobject_cast<QAction*>(sender())) {
		if(QWidget *widget = m_toolBar->widgetForAction(action)) {
			m_colorPicker->showAt(widget->mapToGlobal(QPoint(0, widget->height())));
		}
	}
}

void ChatTab::showContactWindow()
{
	Core::inst()->showContactWindow(m_chat->contacts().first());
}

void ChatTab::showHistoryWindow()
{
	Core::inst()->historyWindow()->showContact(m_chat->contacts().first());
}

void ChatTab::checkDequeue(quint32 msgId, const KittySDK::IMessage &msg)
{
	if(m_ui->textEdit->hasFocus()) {
		Core::inst()->messageQueue()->dequeue(msgId);
	}
}

void ChatTab::editFocused()
{
	Core::inst()->messageQueue()->dequeue(m_chat);
}

}
