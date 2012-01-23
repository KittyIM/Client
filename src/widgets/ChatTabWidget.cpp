#include "ChatTabWidget.h"

#include "SDK/constants.h"
#include "SDK/Contact.h"
#include "SDK/Chat.h"
#include "ChatTab.h"
#include "Core.h"

#include <QtCore/QDebug>
#include <QtGui/QToolButton>
#include <QtGui/QKeyEvent>
#include <QtGui/QTabBar>
#include <QtGui/QMenu>

#define qDebug() qDebug() << "[ChatTabWidget]"
#define qWarning() qWarning() << "[ChatTabWidget]"

using namespace Kitty;
using namespace KittySDK;

Kitty::ChatTabWidget::ChatTabWidget(QWidget *parent): QTabWidget(parent)
{
	m_closedButton = new QToolButton(this);
	m_closedButton->setAutoRaise(true);
	m_closedButton->setToolTip(tr("Recently closed tabs"));
	m_closedButton->setIcon(Core::inst()->icon(Icons::I_DELETE));
	connect(m_closedButton, SIGNAL(clicked()), SLOT(showRecentlyClosed()));
	setCornerWidget(m_closedButton);

	applySettings();
}

Kitty::ChatTabWidget::~ChatTabWidget()
{
	qDeleteAll(m_tabs);
}

Kitty::ChatTab *Kitty::ChatTabWidget::tabByChat(KittySDK::Chat *chat)
{
	foreach(ChatTab *tab, m_tabs) {
		if(tab->chat() == chat) {
			return tab;
		}
	}

	return 0;
}

int Kitty::ChatTabWidget::indexByChat(KittySDK::Chat *chat)
{
	for(int i = 0; i < count(); i++) {
		ChatTab *tab = qobject_cast<ChatTab*>(widget(i));
		if(tab->chat() == chat) {
			return i;
		}
	}

	return -1;
}

QString Kitty::ChatTabWidget::createLabel(Chat *chat)
{
	QString label = Core::inst()->setting(Settings::S_CHATTAB_CAPTION, "%display%").toString();
	Contact *cnt = chat->contacts().first();

	label.replace("%display%", cnt->display());
	label.replace("%status%", Core::inst()->statusToString(cnt->status()));

	if(cnt->description().length() > 0) {
		label.replace("%description%", QString("\"%1\"").arg(cnt->description()));
	} else {
		label.replace("%description%", "");
	}

	label.replace("%unread%", QString::number(0));
	label.replace("%uid%", cnt->uid());
	label.replace("%nickname%", cnt->data(ContactInfos::I_NICKNAME).toString());
	label.replace("%firstname%", cnt->data(ContactInfos::I_FIRSTNAME).toString());
	label.replace("%lastname%", cnt->data(ContactInfos::I_LASTNAME).toString());

	int sex = cnt->data(ContactInfos::I_SEX).toInt();
	if(sex == 0) {
		label.replace("%sex%", tr("Unknown"));
	} else if(sex == 1) {
		label.replace("%sex%", tr("Female"));
	} else {
		label.replace("%sex%", tr("Male"));
	}

	return label;
}

ChatTab *Kitty::ChatTabWidget::startChat(Chat *chat)
{
	ChatTab *chatTab = tabByChat(chat);

	if(!chatTab) {
		chatTab = new ChatTab(chat, this);

		connect(chatTab, SIGNAL(tabChanged()), this, SLOT(changeTab()));

		m_tabs.append(chatTab);
	}

	return chatTab;
}

void Kitty::ChatTabWidget::setCurrentIndex(int index)
{
	QTabWidget::setCurrentIndex(index);

	if(widget(index)) {
		qobject_cast<ChatTab*>(widget(index))->setEditFocus();
	}
}

void Kitty::ChatTabWidget::applySettings()
{
	Core *core = Core::inst();

	updateTabBar();

	for(int i = 0; i < count(); i++) {
		if(ChatTab *tab = qobject_cast<ChatTab*>(widget(i))) {
			setTabText(i, createLabel(tab->chat()));

			tab->applySettings();
		}
	}

	switch(core->setting(Settings::S_CHATWINDOW_TABBAR_POS).toInt()) {
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

void Kitty::ChatTabWidget::updateTabBar()
{
	if(Core::inst()->setting(Settings::S_CHATWINDOW_TABBAR_HIDE_ONE, true).toBool()) {
		if(count() == 1) {
			tabBar()->hide();
		} else {
			tabBar()->show();
		}
	} else {
		tabBar()->show();
	}
}

void Kitty::ChatTabWidget::updateIcons()
{
	m_closedButton->setIcon(Core::inst()->icon(Icons::I_DELETE));

	for(int i = 0; i < count(); i++) {
		if(ChatTab *tab = qobject_cast<ChatTab*>(widget(i))) {
			tab->updateIcons();
		}
	}
}

void Kitty::ChatTabWidget::changeTab()
{
	if(ChatTab *tab = qobject_cast<ChatTab*>(sender())) {
		int i = indexByChat(tab->chat());

		updateTab(i);
	}
}


void Kitty::ChatTabWidget::updateTab(int i)
{
	if(ChatTab *tab = qobject_cast<ChatTab*>(widget(i))) {
		Contact *cnt = tab->chat()->contacts().first();
		Protocol *proto = cnt->protocol();

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

	foreach(Chat *chat, m_closedTabs) {
		QIcon icon;

		Contact *cnt = chat->contacts().first();
		if(cnt) {
			Protocol *proto = cnt->protocol();
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
		Chat *chat = m_closedTabs.takeAt(index);
		m_closedButton->setEnabled(m_closedTabs.count());
		switchTo(chat);
	}
}

void Kitty::ChatTabWidget::tabInserted(int index)
{
	updateTab(index);
	updateTabBar();
}

void Kitty::ChatTabWidget::tabRemoved(int index)
{
	updateTabBar();
}

void Kitty::ChatTabWidget::mousePressEvent(QMouseEvent *event)
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

void Kitty::ChatTabWidget::switchTo(KittySDK::Chat *chat)
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
