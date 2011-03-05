#ifndef ICONTHEME_H
#define ICONTHEME_H

#include <QtCore/QString>
#include <QtCore/QMap>

namespace Kitty
{
  class IconTheme
  {
    public:
      IconTheme(const QString &name);

      QString name() { return m_name; }
      QString author() { return m_author; }
      QString email() { return m_email; }
      QMap<QString, QString> icons() { return m_icons; }


    private:
      QString m_name;
      QString m_author;
      QString m_email;
      QMap<QString, QString> m_icons;
  };
}

#endif // ICONTHEME_H
