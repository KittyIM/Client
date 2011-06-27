#ifndef CONTACTMANAGER_H
#define CONTACTMANAGER_H

#include "SDK/Contact.h"
#include "Singleton.h"

namespace Kitty
{
  class ContactManager: public QObject, public Singleton<ContactManager>
  {
    Q_OBJECT

    friend class Singleton<ContactManager>;

    public:
      const QList<KittySDK::Contact*> &contacts() const;
      const QList<KittySDK::Contact*> contactsByProtocol(const QString &proto);
      const QList<KittySDK::Contact*> contactsByProtocol(KittySDK::Protocol *proto);
      const QList<KittySDK::Contact*> contactsByAccount(const QString &acc, const QString &proto);

      const QStringList groups() const;

      void load(const QString &profile);
      void save(const QString &profile);

    public slots:
      void add(KittySDK::Contact *contact);

    signals:
      void contactAdded(KittySDK::Contact *contact);
      void statusUpdated();

    private:
      ContactManager(QObject *parent = 0): QObject(parent) { }
      ~ContactManager();

    private:
      QList<KittySDK::Contact*> m_contacts;
  };
}

#endif // CONTACTMANAGER_H
