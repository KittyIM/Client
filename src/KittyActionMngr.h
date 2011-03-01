#ifndef KITTYACTIONMNGR_H
#define KITTYACTIONMNGR_H

#include <QtCore/QObject>
#include <QtCore/QMap>

class QAction;

class KittyActionMngr: public QObject
{
  Q_OBJECT

  public:
    KittyActionMngr(QObject *parent = 0);

    void loadDefaults();

    void addAction(const int &id, QAction *action);
    QAction *getAction(const int &id);

  private:
    QMap<int, QAction*> m_actions;
};

#endif // KITTYACTIONMNGR_H
