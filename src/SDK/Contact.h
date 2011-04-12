#ifndef SDKCONTACT_H
#define SDKCONTACT_H

#include "Account.h"

#include <QtCore/QString>

namespace KittySDK
{
  class Message;

  class Contact: public QObject
  {
    Q_OBJECT
    Q_PROPERTY(QString uid READ uid WRITE setUid)
    Q_PROPERTY(QString display READ display WRITE setDisplay)
    Q_PROPERTY(QString group READ group WRITE setGroup)
    Q_PROPERTY(KittySDK::Protocol::Status status READ status WRITE setStatus)

    public:
      Contact(const QString &uid, KittySDK::Account *account): QObject(account), m_uid(uid), m_account(account) { }

      QString uid() const { return m_uid; }
      void setUid(const QString &uid) { m_uid = uid; }

      QString display() const { return m_display; }
      void setDisplay(const QString &display) { m_display = display; }

      QString group() const { return m_group; }
      void setGroup(const QString &group) { m_group = group; }

      KittySDK::Protocol::Status status() const { return m_status; }
      void setStatus(KittySDK::Protocol::Status status) { m_status = status; }

      QString description() const { return m_description; }
      void setDescription(const QString &description) { m_description = description; }

      KittySDK::Account *account() const { return m_account; }

    signals:
      void statusChanged(KittySDK::Protocol::Status status, const QString &description);
      void messageReceived(KittySDK::Message *msg);

    public slots:
      virtual void prepareContextMenu(QMenu *menu) { }
      virtual void loadSettings(const QMap<QString, QVariant> &settings) { }
      virtual QMap<QString, QVariant> saveSettings() { return QMap<QString, QVariant>(); }

    protected:
      QString m_uid;
      QString m_display;
      QString m_group;
      KittySDK::Protocol::Status m_status;
      QString m_description;
      KittySDK::Account *m_account;
  };
}

#endif // SDKCONTACT_H
