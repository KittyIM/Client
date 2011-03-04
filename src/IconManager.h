#ifndef ICONMANAGER_H
#define ICONMANAGER_H

#include <QtCore/QObject>
#include <QtCore/QHash>
#include <QtGui/QPixmap>

class QPixmap;

namespace Kitty
{
  class IconManager: public QObject
  {
    Q_OBJECT

    public:
      explicit IconManager(QObject *parent = 0);

      void loadDefaults();
      QStringList keys() { return m_icons.keys(); }

      void insert(const QString &id, const QPixmap &pixmap, bool replace = true);
      QPixmap icon(const QString &id) { return m_icons.value(id); }

    private:
      QHash<QString, QPixmap> m_icons;
  };
}

#endif // ICONMANAGER_H
