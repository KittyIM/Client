#include "ChatTabWidget.h"

#include "SDK/constants.h"
#include "ChatTab.h"
#include "Core.h"

#include <QtCore/QDebug>
#include <QtGui/QTabBar>
#include <QtGui/QKeyEvent>

using namespace Kitty;
using namespace KittySDK;

Kitty::ChatTabWidget::ChatTabWidget(QWidget *parent): QTabWidget(parent)
{
}

void Kitty::ChatTabWidget::setCurrentIndex(int index)
{
  QTabWidget::setCurrentIndex(index);

  if(widget(index)) {
    static_cast<ChatTab*>(widget(index))->setEditFocus();
  }
}

void Kitty::ChatTabWidget::updateTabBar()
{
  Core *core = Core::inst();

  if(core->setting(Settings::S_CHATWINDOW_TABBAR_HIDE_ONE, true).toBool()) {
    if(count() == 1) {
      tabBar()->hide();
    } else {
      tabBar()->show();
    }
  } else {
    tabBar()->show();
  }
}

void Kitty::ChatTabWidget::tabInserted(int index)
{
  updateTabBar();
}

void Kitty::ChatTabWidget::tabRemoved(int index)
{
  updateTabBar();
}

void Kitty::ChatTabWidget::mousePressEvent(QMouseEvent *event)
{
  if(event->button() == Qt::MiddleButton) {
    emit tabCloseRequested(tabBar()->tabAt(event->pos()));
  }

  QTabWidget::mousePressEvent(event);
}
