#include "ChatTabWidget.h"

#include "SDK/constants.h"
#include "SDK/Contact.h"
#include "SDK/Chat.h"
#include "ChatTab.h"
#include "Core.h"

#include <QtCore/QDebug>
#include <QtGui/QTabBar>
#include <QtGui/QKeyEvent>

#define qDebug() qDebug() << "[ChatTabWidget]"
#define qWarning() qWarning() << "[ChatTabWidget]"

using namespace Kitty;
using namespace KittySDK;

Kitty::ChatTabWidget::ChatTabWidget(QWidget *parent): QTabWidget(parent)
{
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
    ChatTab *tab = static_cast<ChatTab*>(widget(i));
    if(tab->chat() == chat) {
      return i;
    }
  }

  return -1;
}

QString Kitty::ChatTabWidget::createLabel(Chat *chat)
{
  QString label = Core::inst()->setting(Settings::S_CHATTAB_CAPTION, "%nick%").toString();
  Contact *cnt = chat->contacts().first();

  label.replace("%nick%", cnt->display());
  label.replace("%status%", Core::inst()->statusToString(cnt->status()));
  label.replace("%description%", cnt->description());
  label.replace("%unread%", QString::number(0));

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
    static_cast<ChatTab*>(widget(index))->setEditFocus();
  }
}

void Kitty::ChatTabWidget::applySettings()
{
  Core *core = Core::inst();

  updateTabBar();

  for(int i = 0; i < count(); i++) {
    ChatTab *tab = qobject_cast<ChatTab*>(widget(i));

    setTabText(i, createLabel(tab->chat()));

    tab->applySettings();
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
  for(int i = 0; i < count(); i++) {
    ChatTab *tab = qobject_cast<ChatTab*>(widget(i));
    tab->updateIcons();
  }
}

void Kitty::ChatTabWidget::changeTab()
{
  ChatTab *tab = qobject_cast<ChatTab*>(sender());
  int i = indexByChat(tab->chat());

  updateTab(i);
}


void Kitty::ChatTabWidget::updateTab(int i)
{
  ChatTab *tab = qobject_cast<ChatTab*>(widget(i));

  Contact *cnt = tab->chat()->contacts().first();
  Protocol *proto = cnt->protocol();

  setTabIcon(i, Core::inst()->icon(proto->statusIcon(cnt->status())));
  setTabText(i, createLabel(tab->chat()));

  emit currentChanged(i);
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

void Kitty::ChatTabWidget::switchTo(KittySDK::Chat *chat)
{
  int index = indexByChat(chat);

  if(index == -1) {
    ChatTab *chatTab = tabByChat(chat);
    int i = addTab(chatTab, "");

    setCurrentIndex(i);
  } else {
    setCurrentIndex(index);
  }
}
