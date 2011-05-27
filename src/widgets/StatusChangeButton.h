#ifndef STATUSCHANGEBUTTON_H
#define STATUSCHANGEBUTTON_H

#include "SDK/Protocol.h"

#include <QtGui/QToolButton>

namespace Kitty
{
  class StatusChangeButton: public QToolButton
  {
    Q_OBJECT

    public:
      explicit StatusChangeButton(QWidget *parent = 0);

      QSize sizeHint() const;

    signals:
      void statusChanged(KittySDK::Protocol::Status status);

    private slots:
      void setStatusOnline();
      void setStatusAway();
      void setStatusFFC();
      void setStatusDND();
      void setStatusInvisible();
      void setStatusOffline();

    protected:
      void paintEvent(QPaintEvent *event);
  };
}

#endif
