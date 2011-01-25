#ifndef KITTYAPP_H
#define KITTYAPP_H

#include <QtGui/QApplication>

class KittyApp: public QApplication
{
  Q_OBJECT

  public:
    explicit KittyApp(int &argc, char **argv);

  signals:

  private slots:
    void slotCleanUp();

};

#endif // KITTYAPP_H
