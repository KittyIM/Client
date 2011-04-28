#include "ChatManager.h"

#include "widgets/windows/ChatWindow.h"
#include "SDK/Contact.h"
#include "Core.h"

using namespace Kitty;

const QList<KittySDK::Chat*> &Kitty::ChatManager::chats() const
{
  return m_chats;
}

const QList<KittySDK::Chat*> Kitty::ChatManager::chatsByAccount(KittySDK::Account *account) const
{
  QList<KittySDK::Chat*> chats;

  foreach(KittySDK::Chat *chat, m_chats) {
    if(chat->account() == account) {
      chats.append(chat);
    }
  }

  return chats;
}

KittySDK::Chat *Kitty::ChatManager::chat(KittySDK::Contact *me, const QList<KittySDK::Contact*> &contacts) const
{
  foreach(KittySDK::Chat *chat, chatsByAccount(me->account())) {
    if(chat->contacts() == contacts) {
      return chat;
    }
  }

  return 0;
}

void Kitty::ChatManager::startChat(KittySDK::Contact *me, const QList<KittySDK::Contact*> &contacts)
{
  KittySDK::Chat *ch = chat(me, contacts);
  if(!ch) {
    ch = new KittySDK::Chat(me, contacts);
    m_chats.append(ch);

    Core::inst()->chatWindow()->startChat(ch);
  } else {
    Core::inst()->chatWindow()->switchTo(ch);
  }

  Core::inst()->chatWindow()->show();
  Core::inst()->chatWindow()->activateWindow();
}