#include "ChatManager.h"

#include "widgets/windows/ChatWindow.h"
#include "SDK/Contact.h"
#include "Core.h"

using namespace Kitty;
using namespace KittySDK;

const QList<Chat*> &Kitty::ChatManager::chats() const
{
  return m_chats;
}

const QList<Chat*> Kitty::ChatManager::chatsByAccount(Account *account) const
{
  QList<Chat*> chats;

  foreach(Chat *chat, m_chats) {
    if(chat->account() == account) {
      chats.append(chat);
    }
  }

  return chats;
}

Chat *Kitty::ChatManager::chat(Contact *me, const QList<Contact*> &contacts) const
{
  foreach(Chat *chat, chatsByAccount(me->account())) {
    if(chat->contacts() == contacts) {
      return chat;
    }
  }

  return 0;
}

void Kitty::ChatManager::startChat(Contact *me, const QList<Contact*> &contacts)
{
  Chat *ch = chat(me, contacts);
  if(!ch) {
    ch = new Chat(me, contacts);
    m_chats.append(ch);
  }

  Core::inst()->chatWindow()->startChat(ch);
  Core::inst()->chatWindow()->show();
  Core::inst()->chatWindow()->activateWindow();
}
