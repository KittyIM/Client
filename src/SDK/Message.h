#ifndef SDKMESSAGE_H
#define SDKMESSAGE_H

#include <QtCore/QDateTime>
#include <QtCore/QObject>

namespace KittySDK
{
  class Contact;
  class Chat;

  class Message: public QObject
  {
    Q_OBJECT
    Q_PROPERTY(Type type READ type WRITE setType)
    Q_PROPERTY(Direction direction READ direction WRITE setDirection)
    Q_PROPERTY(QString body READ body WRITE setBody)
    Q_PROPERTY(QDateTime timeStamp READ timeStamp WRITE setTimeStamp)

    public:
      enum Type
      {
        Private = 0,
        Conference
      };

      enum Direction
      {
        Incoming = 0,
        Outgoing,
        System
      };

    public:
      Message(Contact *from, Contact *to, const QString &body = "", const QDateTime &timeStamp = QDateTime::currentDateTime(), const Type &type = Private, const Direction &dir = Outgoing): QObject(0),
        m_from(from),
        m_to(QList<Contact*>() << to),
        m_body(body),
        m_timeStamp(timeStamp),
        m_type(type),
        m_direction(dir)
      { }

      Message(Contact *from, const QList<Contact*> &to, const QString &body = "", const QDateTime &timeStamp = QDateTime::currentDateTime(), const Type &type = Private, const Direction &dir = Outgoing): QObject(0),
        m_from(from),
        m_to(to),
        m_body(body),
        m_timeStamp(timeStamp),
        m_type(type),
        m_direction(dir)
      { }

      Contact *from() const { return m_from; }
      const QList<Contact*> &to() const { return m_to; }
      Contact *singleTo() const { return m_to.first(); }

      Chat *chat() const { return m_chat; }
      void setChat(Chat *chat) { m_chat = chat; }

      QString body() const { return m_body; }
      void setBody(const QString &body) { m_body = body; }

      QDateTime timeStamp() const { return m_timeStamp; }
      void setTimeStamp(const QDateTime &timeStamp) { m_timeStamp = timeStamp; }

      Type type() const { return m_type; }
      void setType(const Type &type) { m_type = type; }

      Direction direction() const { return m_direction; }
      void setDirection(const Direction &dir) { m_direction = dir; }

    private:
      Contact *m_from;
      QList<Contact*> m_to;
      Chat *m_chat;
      QString m_body;
      QDateTime m_timeStamp;
      Type m_type;
      Direction m_direction;
  };
}

#endif // SDKMESSAGE_H
