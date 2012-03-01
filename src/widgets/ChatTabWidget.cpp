#include "ChatTabWidget.h"

#include "MessageQueue.h"
#include "ChatTab.h"
#include "Core.h"

#include <SDKConstants.h>
#include <IContact.h>
#include <IChat.h>

#include <QtCore/QDebug>
#include <QtGui/QToolButton>
#include <QtGui/QKeyEvent>
#include <QtGui/QTabBar>
#include <QtGui/QMenu>

#define qDebug() qDebug() << "[ChatTabWidget]"
#define qWarning() qWarning() << "[ChatTabWidget]"

namespace Kitty
{

ChatTabWidget::ChatTabWidget(QWidget *parent): QTabWidget(parent)
{
	connect(MessageQueue::inst(), SIGNAL(messageEnqueued(quint32,KittySDK::IMessage)), SLOT(setupBlinking(quint32,KittySDK::IMessage)));
	connect(MessageQueue::inst(), SIGNAL(messageDequeued(quint32)), SLOT(unblinkIcon(quint32)));
	connect(this, SIGNAL(currentChanged(int)), SLOT(updateTabFocus(int)));

	m_closedButton = new QToolButton(this);
	m_closedButton->setAutoRaise(true);
	m_closedButton->setToolTip(tr("Recently closed tabs"));
	m_closedButton->setIcon(Core::inst()->icon(KittySDK::Icons::I_DELETE));
	connect(m_closedButton, SIGNAL(clicked()), SLOT(showRecentlyClosed()));
	setCornerWidget(m_closedButton);

	applySettings();
}

ChatTabWidget::~ChatTabWidget()
{
	qDeleteAll(m_tabs);
}

ChatTab *ChatTabWidget::tabByChat(KittySDK::IChat *chat)
{
	foreach(ChatTab *tab, m_tabs) {
		if(tab->chat() == chat) {
			return tab;
		}
	}

	return 0;
}

int ChatTabWidget::indexByChat(KittySDK::IChat *chat)
{
	for(int i = 0; i < count(); ++i) {
		ChatTab *tab = qobject_cast<ChatTab*>(widget(i));
		if(tab->chat() == chat) {
			return i;
		}
	}

	return -1;
}

int ChatTabWidget::indexByChatId(const QString &chatId)
{
	for(int i = 0; i < count(); ++i) {
		ChatTab *tab = qobject_cast<ChatTab*>(widget(i));
		if(tab->chat()->id() == chatId) {
			return i;
		}
	}

	return -1;
}

QString ChatTabWidget::createLabel(KittySDK::IChat *chat)
{
	QString label;

	if(chat->contacts().count() == 1) {
		KittySDK::IContact *cnt = chat->contacts().first();

		label = Core::inst()->setting(KittySDK::Settings::S_CHATTAB_CAPTION, "%display%").toString();
		label.replace("%display%", cnt->display());
		label.replace("%status%", Core::inst()->statusToString(cnt->status()));

		if(cnt->description().length() > 0) {
			label.replace("%description%", QString("\"%1\"").arg(cnt->description()));
		} else {
			label.replace("%description%", "");
		}

		label.replace("%unread%", QString::number(0));
		label.replace("%uid%", cnt->uid());
		label.replace("%nickname%", cnt->data(KittySDK::ContactInfos::I_NICKNAME).toString());
		label.replace("%firstname%", cnt->data(KittySDK::ContactInfos::I_FIRSTNAME).toString());
		label.replace("%lastname%", cnt->data(KittySDK::ContactInfos::I_LASTNAME).toString());

		int sex = cnt->data(KittySDK::ContactInfos::I_SEX).toInt();
		if(sex == 0) {
			label.replace("%sex%", tr("Unknown"));
		} else if(sex == 1) {
			label.replace("%sex%", tr("Female"));
		} else {
			label.replace("%sex%", tr("Male"));
		}
	} else {
		foreach(KittySDK::IContact *cnt, chat->contacts()) {
			label += cnt->display() + ", ";
		}

		label.chop(2);
	}

	return label;
}

ChatTab *ChatTabWidget::startChat(KittySDK::IChat *chat)
{
	ChatTab *chatTab = tabByChat(chat);

	//create if not exists
	if(!chatTab) {
		chatTab = new ChatTab(chat, this);

		connect(chatTab, SIGNAL(tabChanged()), this, SLOT(changeTab()));

		m_tabs.append(chatTab);
	}

	//make sure the tabs is inserted
	if(indexByChat(chat) == -1) {
		addTab(chatTab, "");
	}

	//remove the chat from recently closed tabs
	m_closedTabs.removeAll(chat);
	if(m_closedTabs.count() == 0) {
		m_closedButton->setEnabled(false);
	}

	return chatTab;
}

void ChatTabWidget::setCurrentIndex(int index)
{
	QTabWidget::setCurrentIndex(index);

	if(widget(index)) {
		qobject_cast<ChatTab*>(widget(index))->setEditFocus();
	}
}

void ChatTabWidget::applySettings()
{
	Core *core = Core::inst();

	updateTabBar();

	for(int i = 0; i < count(); ++i) {
		if(ChatTab *tab = qobject_cast<ChatTab*>(widget(i))) {
			setTabText(i, createLabel(tab->chat()));

			tab->applySettings();
		}
	}

	switch(core->setting(KittySDK::Settings::S_CHATWINDOW_TABBAR_POS).toInt()) {
		case 0:
			setTabPosition(QTabWidget::North);
		break;

		case 1:
			setTabPosition(QTabWidget::South);
		break;

		case 2:
			setTabPosition(QTabWidget::West);
		break;

		case 3:
			setTabPosition(QTabWidget::East);
		break;
	}
}

void ChatTabWidget::updateTabBar()
{
	if(Core::inst()->setting(KittySDK::Settings::S_CHATWINDOW_TABBAR_HIDE_ONE, true).toBool()) {
		if(count() == 1) {
			tabBar()->hide();
		} else {
			tabBar()->show();
		}
	} else {
		tabBar()->show();
	}
}

void ChatTabWidget::updateIcons()
{
	m_closedButton->setIcon(Core::inst()->icon(KittySDK::Icons::I_DELETE));

	for(int i = 0; i < count(); ++i) {
		if(ChatTab *tab = qobject_cast<ChatTab*>(widget(i))) {
			tab->updateIcons();
		}
	}
}

void ChatTabWidget::changeTab()
{
	if(ChatTab *tab = qobject_cast<ChatTab*>(sender())) {
		int i = indexByChat(tab->chat());

		updateTab(i);
		emit currentChanged(i);
	}
}


void ChatTabWidget::updateTab(int i)
{
	if(ChatTab *tab = qobject_cast<ChatTab*>(widget(i))) {
		setTabText(i, createLabel(tab->chat()));
	}

	updateTabIcon(i);
}

void ChatTabWidget::updateTabIcon(int i)
{
	if(ChatTab *tab = qobject_cast<ChatTab*>(widget(i))) {
		if(tab->chat()->contacts().count() == 1) {
			KittySDK::IContact *cnt = tab->chat()->contacts().first();
			if(KittySDK::IProtocol *proto = cnt->protocol()) {
				setTabIcon(i, Core::inst()->icon(proto->statusIcon(cnt->status())));
			}
		} else {
			setTabIcon(i, Core::inst()->icon(KittySDK::Icons::I_GROUP_CHAT));
		}
	}
}

void ChatTabWidget::updateTabFocus(int i)
{
	if(ChatTab *tab = qobject_cast<ChatTab*>(widget(i))) {
		tab->setEditFocus();
	}
}

void ChatTabWidget::showRecentlyClosed()
{
	QMenu menu;
	menu.addAction(tr("Clear recently closed tabs list"), this, SLOT(clearRecentlyClosed()));
	menu.addSeparator();

	foreach(KittySDK::IChat *chat, m_closedTabs) {
		QIcon icon;

		if(KittySDK::IContact *cnt = chat->contacts().first()) {
			KittySDK::IProtocol *proto = cnt->protocol();
			if(proto) {
				icon = QIcon(Core::inst()->icon(proto->statusIcon(cnt->status())));
			}
		}

		QString title;
		if(chat->contacts().count() == 1) {
			title = chat->contacts().first()->display();
		} else {
			title = createLabel(chat);
		}

		QAction *act = menu.addAction(icon, title, this, SLOT(restoreClosedTab()));
		act->setData(m_closedTabs.indexOf(chat));

	}

	menu.exec(m_closedButton->mapToGlobal(QPoint(0, m_closedButton->height())));

}

void ChatTabWidget::clearRecentlyClosed()
{
	m_closedTabs.clear();
	m_closedButton->setEnabled(false);
}

void ChatTabWidget::restoreClosedTab()
{
	if(QAction *action = qobject_cast<QAction*>(sender())) {
		int index = action->data().toInt();
		KittySDK::IChat *chat = m_closedTabs.takeAt(index);
		m_closedButton->setEnabled(m_closedTabs.count());
		switchTo(chat);
	}
}

void ChatTabWidget::setupBlinking(quint32 msgId, const KittySDK::IMessage &msg)
{
	if(msg.direction() == KittySDK::IMessage::Incoming) {
		if(!MessageQueue::inst()->isFirstForChat(msg.chat())) {
			return;
		}

		QTimer *blinkTimer = new QTimer(this);
		blinkTimer->setProperty("chatId", msg.chat()->id());
		connect(blinkTimer, SIGNAL(timeout()), SLOT(blinkTab()));
		blinkTimer->start(Core::inst()->setting(KittySDK::Settings::S_BLINKING_SPEED, 500).toInt());

		m_blinkTimers.insert(msgId, blinkTimer);
	}
}

void ChatTabWidget::blinkTab()
{
	if(QTimer *timer = qobject_cast<QTimer*>(sender())) {
		int index = indexByChatId(timer->property("chatId").toString());

		if(timer->property("blink").toBool()) {
			setTabIcon(index, Core::inst()->icon(KittySDK::Icons::I_MESSAGE));
		} else {
			updateTabIcon(index);
		}

		timer->setProperty("blink", !timer->property("blink").toBool());
	}
}

void ChatTabWidget::unblinkIcon(quint32 msgId)
{
	if(m_blinkTimers.contains(msgId)) {
		QTimer *timer = m_blinkTimers.take(msgId);

		int index = indexByChatId(timer->property("chatId").toString());
		updateTab(index);

		delete timer;
	}
}

void ChatTabWidget::tabInserted(int index)
{
	updateTab(index);
	updateTabBar();
}

void ChatTabWidget::tabRemoved(int index)
{
	updateTabBar();
}

void ChatTabWidget::mousePressEvent(QMouseEvent *event)
{
	if(event->button() == Qt::MiddleButton) {
		emit tabCloseRequested(tabBar()->tabAt(event->pos()));
	}

	QTabWidget::mousePressEvent(event);
}

void ChatTabWidget::changeEvent(QEvent *event)
{
	if(event->type() == QEvent::LanguageChange) {
		m_closedButton->setToolTip(tr("Recently closed tabs"));
	}

	QWidget::changeEvent(event);
}

void ChatTabWidget::switchTo(KittySDK::IChat *chat)
{
	int index = indexByChat(chat);

	if(index == -1) {
		ChatTab *chatTab = tabByChat(chat);
		index = addTab(chatTab, "");
	}

	setCurrentIndex(index);
	m_closedTabs.removeAll(chat);
	m_closedButton->setEnabled(m_closedTabs.count());
}

void ChatTabWidget::removeTab(int index)
{
	if(ChatTab *tab = qobject_cast<ChatTab*>(widget(index))) {
		m_closedTabs.removeAll(tab->chat());
		m_closedTabs.append(tab->chat());
		m_closedButton->setEnabled(true);
	}

	QTabWidget::removeTab(index);
}

}
