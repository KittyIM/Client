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
