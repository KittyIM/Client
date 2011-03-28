#ifndef SDKACCOUNT_H
#define SDKACCOUNT_H

#include "Protocol.h"

#include <QtCore/QObject>

namespace KittySDK
{
  class Account: public QObject
  {
    Q_OBJECT
    Q_PROPERTY(QString uid READ uid WRITE setUid)
    Q_PROPERTY(QString password READ password WRITE setPassword)

    public:
      Account(const QString &uid, Protocol *parent): QObject(parent), m_uid(uid) { }

      QString uid() const { return m_uid; }
      void setUid(const QString &uid) { m_uid = uid; }

      QString password() const { return m_password; }
      void setPassword(const QString &password) { m_password = password; }

    protected:
      QString m_uid;
      QString m_password;
  };
}

#endif // SDKACCOUNT_H
