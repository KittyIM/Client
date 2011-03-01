#ifndef KITTYCORE_H
#define KITTYCORE_H

class wndMain;
class KittyActionMngr;
class KittyIconMngr;

class QAction;
class QPixmap;
class QSystemTrayIcon;

class KittyCore
{
  public:
    ~KittyCore();

    static KittyCore *inst();
    static void destroy();

    QAction *getAction(const int &id);
    QPixmap getIcon(const int &id);

    wndMain *getWndMain();
    void showWndMain();

    QSystemTrayIcon *getTrayIcon();
    void showTrayIcon();

  private:
    KittyCore();
    KittyCore(const KittyCore &);
    void operator=(const KittyCore &);

  private:
    static KittyCore *m_inst;
    wndMain *m_wndMain;
    KittyActionMngr *m_mngrAct;
    KittyIconMngr *m_mngrIcon;
    QSystemTrayIcon *m_trayIcon;
};

#endif // KITTYCORE_H
