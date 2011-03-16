#ifndef ICONMANAGER_H
#define ICONMANAGER_H

#include "Singleton.h"

#include <QtCore/QObject>
#include <QtCore/QHash>
#include <QtGui/QPixmap>
#include <QtGui/QIcon>

class QPixmap;

namespace Kitty
{
  class IconManager: public QObject, public Singleton<IconManager>
  {
    Q_OBJECT

    friend class Singleton<IconManager>;

    public:
      void loadDefaults();
      QStringList keys() const { return m_icons.keys(); }

      void insert(const QString &id, const QPixmap &pixmap, bool replace = true);
      QPixmap icon(const QString &id) const { return m_icons.value(id); }

    signals:
      void iconsUpdated();

    private:
      explicit IconManager(QObject *parent = 0);

    private:
      QHash<QString, QPixmap> m_icons;
  };
}

#endif // ICONMANAGER_H
