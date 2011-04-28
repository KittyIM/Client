#ifndef CHATMANAGER_H
#define CHATMANAGER_H

#include "SDK/Chat.h"
#include "Singleton.h"

#include <QtCore/QObject>

namespace Kitty
{
  class ChatManager: public QObject, public Singleton<ChatManager>
  {
    Q_OBJECT

    friend class Singleton<ChatManager>;

    public:
      const QList<KittySDK::Chat*> &chats() const;
      const QList<KittySDK::Chat*> chatsByAccount(KittySDK::Account *account) const;

      KittySDK::Chat *chat(KittySDK::Contact *me, const QList<KittySDK::Contact*> &contacts) const;

      void startChat(KittySDK::Contact *me, const QList<KittySDK::Contact*> &contacts);

    signals:


    private:
      ChatManager(QObject *parent = 0): QObject(parent) { }

    private:
      QList<KittySDK::Chat*> m_chats;
  };
}
#endif // CHATMANAGER_H
