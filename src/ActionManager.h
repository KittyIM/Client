#ifndef ACTIONMANAGER_H
#define ACTIONMANAGER_H

#include <QtCore/QStringList>
#include <QtCore/QPointer>
#include <QtCore/QHash>
#include <QtGui/QAction>

namespace Kitty
{
  class ActionManager: public QObject
  {
    Q_OBJECT

    public:
      explicit ActionManager(QObject *parent = 0);

      void loadDefaults();
      QStringList keys() const { return m_actions.keys(); }

      void insert(const QString &id, QAction *action) { m_actions.insert(id, action); }
      QAction *action(const QString &id) const { return m_actions.value(id); }

    private:
      QHash<QString, QPointer<QAction> > m_actions;
  };
}
#endif // ACTIONMANAGER_H
