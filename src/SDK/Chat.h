#ifndef SDKCHAT_H
#define SDKCHAT_H

#include "Contact.h"

#include <QtCore/QObject>

namespace KittySDK
{
  class Protocol;
  class Message;

  class Chat: public QObject
  {
    Q_OBJECT

    public:
      Chat(Contact *me, const QList<Contact*> &contacts): QObject(me), m_me(me), m_contacts(contacts) { }

      Contact *me() const { return m_me; }
      Account *account() const { return m_me->account(); }
      Protocol *protocol() const { return account()->protocol(); }

      const QList<Contact*> &contacts() const { return m_contacts; }

    private:
      Contact *m_me;
      QList<Contact*> m_contacts;
  };
}

#endif // SDKCHAT_H
