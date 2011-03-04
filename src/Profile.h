#ifndef PROFILE_H
#define PROFILE_H

#include <QtCore/QObject>
#include <QtCore/QString>

namespace Kitty
{
  class XmlSettings;

  class Profile: public QObject
  {
    Q_OBJECT

    public:
      explicit Profile(QObject *parent = 0);
      ~Profile();

      void load(const QString &name);

      QString name() { return m_name; }
      XmlSettings *settings() { return m_settings; }
      bool isLoaded() { return !m_name.isEmpty(); }
      bool hasPassword();

    private:
      QString m_name;
      XmlSettings *m_settings;
  };
}

#endif // PROFILE_H
