#ifndef KITTYCORE_H
#define KITTYCORE_H

#include <QtCore/QObject>
#include <QtGui/QSystemTrayIcon>

class wndMain;
class wndProfiles;
class KittyProfile;
class KittyIconMngr;
class KittyActionMngr;

class QAction;

class KittyCore: public QObject
{
  Q_OBJECT
  Q_PROPERTY(bool restart READ hasToRestart WRITE setRestart)

  public:
    ~KittyCore();

    static KittyCore *inst();
    static void destroy();

    QAction *action(const char *id);
    QAction *action(const QString &id);
    QPixmap icon(const int &id);
    void loadProfile(QString name);

    wndMain *mainWindow();
    wndProfiles *profilesWindow();
    QSystemTrayIcon *trayIcon();
    KittyProfile *profile();
    KittyActionMngr *actionManager() { return m_mngrAct; }
    KittyIconMngr *iconManager() { return m_mngrIcon; }

    void setRestart(bool restart) { m_restart = restart; }
    bool hasToRestart() { return m_restart; }

  public slots:
    void restart();
    void showTrayIcon();
    void showMainWindow();
    void showProfilesWindow();
    void toggleMainWindow();

  private slots:
    void trayIconActivated(QSystemTrayIcon::ActivationReason reason);

  private:
    KittyCore();
    KittyCore(const KittyCore &);
    void operator=(const KittyCore &);

  private:
    static KittyCore *m_inst;
    bool m_restart;
    wndMain *m_wndMain;
    wndProfiles *m_wndProfiles;
    KittyProfile *m_profile;
    KittyActionMngr *m_mngrAct;
    KittyIconMngr *m_mngrIcon;
    QSystemTrayIcon *m_trayIcon;
};

#endif // KITTYCORE_H
