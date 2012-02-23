#include "ChatTabWidget.h"

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

QString ChatTabWidget::createLabel(KittySDK::IChat *chat)
{
	QString label = Core::inst()->setting(KittySDK::Settings::S_CHATTAB_CAPTION, "%display%").toString();
	KittySDK::IContact *cnt = chat->contacts().first();

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

	return label;
}

ChatTab *ChatTabWidget::startChat(KittySDK::IChat *chat)
{
	ChatTab *chatTab = tabByChat(chat);

	if(!chatTab) {
		chatTab = new ChatTab(chat, this);

		connect(chatTab, SIGNAL(tabChanged()), this, SLOT(changeTab()));

		m_tabs.append(chatTab);
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
	}
}


void ChatTabWidget::updateTab(int i)
{
	if(ChatTab *tab = qobject_cast<ChatTab*>(widget(i))) {
		KittySDK::IContact *cnt = tab->chat()->contacts().first();
		KittySDK::IProtocol *proto = cnt->protocol();

		setTabIcon(i, Core::inst()->icon(proto->statusIcon(cnt->status())));
		setTabText(i, createLabel(tab->chat()));
	}

	emit currentChanged(i);
}

void ChatTabWidget::showRecentlyClosed()
{
	QMenu menu;
	menu.addAction(tr("Clear recently closed tabs list"), this, SLOT(clearRecentlyClosed()));
	menu.addSeparator();

	foreach(KittySDK::IChat *chat, m_closedTabs) {
		QIcon icon;

		KittySDK::IContact *cnt = chat->contacts().first();
		if(cnt) {
			KittySDK::IProtocol *proto = cnt->protocol();
			if(proto) {
				icon = QIcon(Core::inst()->icon(proto->statusIcon(cnt->status())));
			}
		}

		QAction *act = menu.addAction(icon, createLabel(chat), this, SLOT(restoreClosedTab()));
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
