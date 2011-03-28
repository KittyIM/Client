#ifndef CONTACTMANAGER_H
#define CONTACTMANAGER_H

#include "Singleton.h"

#include <QtCore/QObject>

namespace Kitty
{
  class ContactManager: public QObject, public Singleton<ContactManager>
  {
    Q_OBJECT

    friend class Singleton<ContactManager>;

    private:
      ContactManager(QObject *parent = 0): QObject(parent) { }
  };
}

#endif // CONTACTMANAGER_H
