#ifndef ABSTRACTTHEME_H
#define ABSTRACTTHEME_H

#include <QtCore/QString>

namespace Kitty
{
  class AbstractTheme
  {
    public:
      AbstractTheme() { }

      QString name() const { return m_name; }
      QString author() const { return m_author; }
      QString email() const { return m_email; }

    protected:
      QString m_name;
      QString m_author;
      QString m_email;
  };
}

#endif // ABSTRACTTHEME_H
