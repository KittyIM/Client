#include "RosterTreeView.h"

#include "widgets/windows/SettingsWindow.h"
#include "RosterItemDelegate.h"
#include "Core.h"

#include <QtCore/QDebug>
#include <QtGui/QMouseEvent>
#include <QtGui/QMenu>

Kitty::RosterTreeView::RosterTreeView(QWidget *parent): QTreeView(parent)
{
  m_delegate = new Kitty::RosterItemDelegate(this);

  setItemDelegate(m_delegate);
}

void Kitty::RosterTreeView::mousePressEvent(QMouseEvent *event)
{
  QTreeView::mousePressEvent(event);

  if(event->button() == Qt::RightButton) {
    QMenu menu(this);
    //menu.addAction(new QAction("Test", &menu));
    menu.exec(event->globalPos());
  }
}
