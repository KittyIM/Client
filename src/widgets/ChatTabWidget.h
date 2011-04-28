#ifndef CHATTABWIDGET_H
#define CHATTABWIDGET_H

#include <QtGui/QTabWidget>

namespace KittySDK
{
  class Chat;
}

namespace Kitty
{
  class ChatTabWidget: public QTabWidget
  {
    Q_OBJECT

    public:
      explicit ChatTabWidget(QWidget *parent = 0);

    public slots:
      void setCurrentIndex(int index);
      void updateTabBar();

    protected:
      void tabInserted(int index);
      void tabRemoved(int index);
      void mousePressEvent(QMouseEvent *event);
  };
}

#endif // CHATTABWIDGET_H
