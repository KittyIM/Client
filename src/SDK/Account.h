#ifndef SDKACCOUNT_H
#define SDKACCOUNT_H

#include "Protocol.h"

#include <QtCore/QDebug>
#include <QtCore/QObject>

class QMenu;

namespace KittySDK
{
  class Contact;
  class Message;

  class Account: public QObject
  {
    Q_OBJECT
    Q_PROPERTY(QString uid READ uid WRITE setUid)
    Q_PROPERTY(QString password READ password WRITE setPassword)

    public:
      Account(const QString &uid, Protocol *parent): QObject(parent), m_uid(uid), m_protocol(parent) { }

      KittySDK::Protocol *protocol() const { return m_protocol; }

      QString uid() const { return m_uid; }
      void setUid(const QString &uid) { m_uid = uid; }

      QString password() const { return m_password; }
      void setPassword(const QString &password) { m_password = password; }

      KittySDK::Contact *me() const { return m_me; }
      void setMe(KittySDK::Contact *me) { m_me = me; }

      const QMap<QString, KittySDK::Contact*> &contacts() { return m_contacts; }
      virtual void insertContact(const QString &uid, KittySDK::Contact *contact) { m_contacts.insert(uid, contact); }

      virtual KittySDK::Contact *newContact(const QString &uid) { return 0; }

      virtual KittySDK::Protocol::Status status() const { return KittySDK::Protocol::Offline; }

    signals:
      void statusChanged();
      void messageReceived(KittySDK::Message &msg);

    public slots:
      virtual void loadSettings(const QMap<QString, QVariant> &settings) { }
      virtual QMap<QString, QVariant> saveSettings() { return QMap<QString, QVariant>(); }
      virtual QMenu *statusMenu() { return 0; }
      virtual void sendMessage(const KittySDK::Message &msg) { }

    protected:
      QString m_uid;
      QString m_password;
      KittySDK::Contact *m_me;
      QMap<QString, KittySDK::Contact*> m_contacts;
      KittySDK::Protocol *m_protocol;
  };
}

#endif // SDKACCOUNT_H
