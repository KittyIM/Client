#ifndef ICONTHEME_H
#define ICONTHEME_H

#include "AbstractTheme.h"

#include <QtCore/QMap>

namespace Kitty
{
  class IconTheme: public AbstractTheme
  {
    public:
      IconTheme(const QString &name);

      QMap<QString, QString> icons() const { return m_icons; }

    private:
      QMap<QString, QString> m_icons;
  };
}

#endif // ICONTHEME_H
