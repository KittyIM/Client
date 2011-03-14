#ifndef CORE_H
#define CORE_H

#include "Singleton.h"

#include <QtCore/QVariant>
#include <QtCore/QObject>
#include <QtGui/QSystemTrayIcon>

class QAction;

namespace Kitty
{
  class ProfilesWindow;
  class SettingsWindow;
  class AboutWindow;
  class XmlSettings;
  class MainWindow;
  class Profile;


  class Core: public QObject, public Singleton<Core>
  {
    Q_OBJECT
    Q_PROPERTY(bool restart READ hasToRestart WRITE setRestart)
    Q_PROPERTY(bool portable READ isPortable WRITE setPortable)

    friend class Singleton<Core>;

    public:
      ~Core();

      QAction *action(const QString &id) const;
      QPixmap icon(const QString &id) const;
      QVariant setting(const QString &key, const QVariant &defaultValue = QVariant());

      void setSetting(const QString &key, const QVariant &value);
      void loadProfile(const QString &name);

      MainWindow *mainWindow();
      AboutWindow *aboutWindow();
      ProfilesWindow *profilesWindow();
      SettingsWindow *settingsWindow();
      QSystemTrayIcon *trayIcon();
      Profile *profile();
      XmlSettings *settings();

      QString profilesDir() const;

      void setRestart(bool restart) { m_restart = restart; }
      bool hasToRestart() const { return m_restart; }

      void setPortable(bool portable) { m_portable = portable; }
      bool isPortable() const { return m_portable; }

    public slots:
      void restart();
      void showTrayIcon();
      void showAboutWindow();
      void showMainWindow();
      void showProfilesWindow();
      void showSettingsWindow();
      void toggleMainWindow();

    private slots:
      void trayIconActivated(QSystemTrayIcon::ActivationReason reason);

    private:
      Core();
      Core(const Core &);
      void operator=(const Core &);

    private:
      bool m_restart;
      bool m_portable;
      ProfilesWindow *m_profilesWindow;
      SettingsWindow *m_settingsWindow;
      QSystemTrayIcon *m_trayIcon;
      AboutWindow *m_aboutWindow;
      MainWindow *m_mainWindow;
      Profile *m_profile;
  };
}

#endif // CORE_H
