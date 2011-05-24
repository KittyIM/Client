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
      const QStringList groups() const;

      void add(KittySDK::Contact *contact);

      void load(const QString &profile);
      void save(const QString &profile);

    signals:
      void contactAdded(KittySDK::Contact *contact);
      void statusUpdated();

    private:
      ContactManager(QObject *parent = 0): QObject(parent) { }

    private:
      QList<KittySDK::Contact*> m_contacts;
  };
}

#endif // CONTACTMANAGER_H
