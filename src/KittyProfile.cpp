#include "KittyProfile.h"

KittyProfile::KittyProfile(QObject *parent): QObject(parent)
{
}

void KittyProfile::load(QString name)
{
  m_name = name;
}
