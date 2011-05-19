#ifndef STATUSCHANGEBUTTON_H
#define STATUSCHANGEBUTTON_H

#include <QtGui/QToolButton>

namespace Kitty
{
  class StatusChangeButton: public QToolButton
  {
    Q_OBJECT

    public:
      explicit StatusChangeButton(QWidget *parent = 0);

      QSize sizeHint() const;

    protected:
      void paintEvent(QPaintEvent *event);
  };
}

#endif
