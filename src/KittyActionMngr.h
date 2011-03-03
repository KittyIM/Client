#ifndef KITTYACTIONMNGR_H
#define KITTYACTIONMNGR_H

#include <QtCore/QHash>
#include <QtCore/QPointer>
#include <QtCore/QStringList>
#include <QtGui/QAction>

class KittyActionMngr: public QObject
{
  Q_OBJECT

  public:
    KittyActionMngr(QObject *parent = 0);

    void loadDefaults();
    QStringList keys() { return m_actions.keys(); }

    void insert(const char *id, QAction *action) { m_actions.insert(id, action); }
    QAction *action(const char *id) { return m_actions.value(id); }
    QAction *action(const QString &id) { return m_actions.value(id); }

  private:
    QHash<QString, QPointer<QAction> > m_actions;
};

#endif // KITTYACTIONMNGR_H
