#ifndef CORE_H
#define CORE_H

#include <QtCore/QVariant>
#include <QtCore/QObject>
#include <QtGui/QSystemTrayIcon>

class QAction;

namespace Kitty
{
  class ProfilesWindow;
  class ActionManager;
  class IconManager;
  class XmlSettings;
  class MainWindow;
  class Profile;


  class Core: public QObject
  {
    Q_OBJECT
    Q_PROPERTY(bool restart READ hasToRestart WRITE setRestart)
    Q_PROPERTY(bool portable READ isPortable WRITE setPortable)

    public:
      ~Core();

      static Core *inst();
      static void destroy();

      QAction *action(const QString &id);
      QPixmap icon(const QString &id);
      QVariant setting(const QString &key, const QVariant &defaultValue = QVariant());

      void setSetting(const QString &key, const QVariant &value);
      void loadProfile(QString name);

      MainWindow *mainWindow();
      ProfilesWindow *profilesWindow();
      QSystemTrayIcon *trayIcon();
      Profile *profile();
      XmlSettings *settings();
      ActionManager *actionManager() { return m_mngrAct; }
      IconManager *iconManager() { return m_mngrIcon; }

      QString profilesDir();

      void setRestart(bool restart) { m_restart = restart; }
      bool hasToRestart() { return m_restart; }

      void setPortable(bool portable) { m_portable = portable; }
      bool isPortable() { return m_portable; }

    public slots:
      void restart();
      void showTrayIcon();
      void showMainWindow();
      void showProfilesWindow();
      void toggleMainWindow();

    private slots:
      void trayIconActivated(QSystemTrayIcon::ActivationReason reason);

    private:
      Core();
      Core(const Core &);
      void operator=(const Core &);

    private:
      static Core *m_inst;
      bool m_restart;
      bool m_portable;
      MainWindow *m_wndMain;
      ProfilesWindow *m_wndProfiles;
      Profile *m_profile;
      ActionManager *m_mngrAct;
      IconManager *m_mngrIcon;
      QSystemTrayIcon *m_trayIcon;
  };
}

#endif // CORE_H
