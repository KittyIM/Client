#ifndef KITTYICONMNGR_H
#define KITTYICONMNGR_H

#include <QtCore/QObject>
#include <QtCore/QMap>
#include <QtGui/QPixmap>

class QPixmap;

class KittyIconMngr: public QObject
{
  Q_OBJECT

  public:
    KittyIconMngr(QObject *parent = 0);

    void loadDefaults();

    void addIcon(const int &id, const QPixmap &pixmap, bool replace = true);
    QPixmap getIcon(const int &id);

  private:
    QMap<int, QPixmap> m_icons;
};

#endif // KITTYICONMNGR_H
