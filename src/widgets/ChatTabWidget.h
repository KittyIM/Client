#ifndef CHATTABWIDGET_H
#define CHATTABWIDGET_H

#include <QtGui/QTabWidget>

namespace Kitty
{
  class ChatTabWidget: public QTabWidget
  {
    Q_OBJECT

    public:
      explicit ChatTabWidget(QWidget *parent = 0);

    protected:
      void tabRemoved(int index);
      void mousePressEvent(QMouseEvent *event);
  };
}

#endif // CHATTABWIDGET_H
