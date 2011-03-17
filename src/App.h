#ifndef APP_H
#define APP_H

#include <QtCore/QDateTime>
#include <QtGui/QApplication>


namespace Kitty
{
  class App: public QApplication
  {
    Q_OBJECT

    public:
      explicit App(int &argc, char **argv);

      QDateTime startDate() { return m_startDate; }

    public slots:
      void applySettings();

    private slots:
      void cleanUp();

    private:
      QDateTime m_startDate;
  };
}

#endif // APP_H
