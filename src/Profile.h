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

      void load(const QString &name, bool settingsOnly = false);

      QString name() const { return m_name; }
      XmlSettings *settings() const { return m_settings; }
      bool isLoaded() const { return !m_name.isEmpty(); }
      bool hasPassword() const;

    private:
      void loadIconTheme(const QString &name);

    private:
      QString m_name;
      XmlSettings *m_settings;
  };
}

#endif // PROFILE_H
