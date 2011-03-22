#include "ChatTabWidget.h"

#include <QtCore/QDebug>
#include <QtGui/QTabBar>
#include <QtGui/QKeyEvent>

Kitty::ChatTabWidget::ChatTabWidget(QWidget *parent): QTabWidget(parent)
{
}

void Kitty::ChatTabWidget::tabRemoved(int index)
{
  QTabWidget::tabRemoved(index);

  if(count() > 1) {
    tabBar()->show();
  } else {
    tabBar()->hide();
  }
}

void Kitty::ChatTabWidget::mousePressEvent(QMouseEvent *event)
{
  if(event->button() == Qt::MiddleButton) {
    emit tabCloseRequested(tabBar()->tabAt(event->pos()));
  }

  QTabWidget::mousePressEvent(event);
}
