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

      void add(KittySDK::Contact *contact);

      void load(const QString &profile);

    signals:
      void contactAdded();

    private slots:
      void updateStatuses();

    private:
      ContactManager(QObject *parent = 0): QObject(parent) { }

    private:
      QList<KittySDK::Contact*> m_contacts;
  };
}

#endif // CONTACTMANAGER_H
