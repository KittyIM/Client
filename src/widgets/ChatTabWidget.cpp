#include "ChatTabWidget.h"

#include "ChatTab.h"

#include <QtCore/QDebug>
#include <QtGui/QTabBar>
#include <QtGui/QKeyEvent>

Kitty::ChatTabWidget::ChatTabWidget(QWidget *parent): QTabWidget(parent)
{
}

void Kitty::ChatTabWidget::switchTo(KittySDK::Chat *chat)
{
  for(int i = 0; i < count(); i++) {
    Kitty::ChatTab *tab = static_cast<Kitty::ChatTab*>(widget(i));
    if(tab->chat() == chat) {
      setCurrentIndex(i);

      return;
    }
  }
}

void Kitty::ChatTabWidget::setCurrentIndex(int index)
{
  QTabWidget::setCurrentIndex(index);

  static_cast<Kitty::ChatTab*>(widget(index))->setEditFocus();
}

void Kitty::ChatTabWidget::tabRemoved(int index)
{
  QTabWidget::tabRemoved(index);

  if(count() > 1) {
    //tabBar()->show();
  } else {
    //tabBar()->hide();
  }
}

void Kitty::ChatTabWidget::mousePressEvent(QMouseEvent *event)
{
  if(event->button() == Qt::MiddleButton) {
    emit tabCloseRequested(tabBar()->tabAt(event->pos()));
  }

  QTabWidget::mousePressEvent(event);
}
