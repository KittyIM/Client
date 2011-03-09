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

      QString name() const { return m_name; }
      QString author() const { return m_author; }
      QString email() const { return m_email; }
      QMap<QString, QString> icons() const { return m_icons; }

    private:
      QString m_name;
      QString m_author;
      QString m_email;
      QMap<QString, QString> m_icons;
  };
}

#endif // ICONTHEME_H
