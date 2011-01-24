#ifndef KITTYAPP_H
#define KITTYAPP_H

#include <QtGui/QApplication>

class KittyApp: public QApplication
{
  Q_OBJECT

  public:
    explicit KittyApp(int &argc, char **argv);

  signals:

  public slots:

};

#endif // KITTYAPP_H
