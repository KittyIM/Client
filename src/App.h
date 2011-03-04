#ifndef APP_H
#define APP_H

#include <QtGui/QApplication>

namespace Kitty
{
  class App: public QApplication
  {
    Q_OBJECT

    public:
      explicit App(int &argc, char **argv);

    signals:

    private slots:
      void cleanUp();

  };
}

#endif // APP_H
