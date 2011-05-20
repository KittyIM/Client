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
      KittySDK::Chat *chat(KittySDK::Contact *me, KittySDK::Contact *sender) const;

      void startChat(KittySDK::Contact *me, const QList<KittySDK::Contact*> &contacts);

    public slots:
      void receiveMessage(KittySDK::Message &msg);

    private:
      ChatManager(QObject *parent = 0): QObject(parent) { }
      ~ChatManager();

    private:
      QList<KittySDK::Chat*> m_chats;
  };
}
#endif // CHATMANAGER_H
