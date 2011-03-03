#ifndef KITTYICONMNGR_H
#define KITTYICONMNGR_H

#include <QtCore/QObject>
#include <QtCore/QHash>
#include <QtGui/QPixmap>

class QPixmap;

class KittyIconMngr: public QObject
{
  Q_OBJECT

  public:
    KittyIconMngr(QObject *parent = 0);

    void loadDefaults();

    void insert(const int &id, const QPixmap &pixmap, bool replace = true);
    QPixmap icon(const int &id);

  private:
    QHash<int, QPixmap> m_icons;
};

#endif // KITTYICONMNGR_H
