#ifndef KITTYCORE_H
#define KITTYCORE_H

class wndMain;

class KittyCore
{
  public:
    ~KittyCore();

    static KittyCore *inst();
    static void destroy();

    wndMain *getWndMain();
    void showWndMain();

  private:
    KittyCore();
    KittyCore(const KittyCore &);
    void operator=(const KittyCore &);

  private:
    static KittyCore *m_inst;
    wndMain *m_wndMain;
};

#endif // KITTYCORE_H
