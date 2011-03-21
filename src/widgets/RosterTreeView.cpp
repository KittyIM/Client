#include "RosterTreeView.h"

#include "RosterItemDelegate.h"

Kitty::RosterTreeView::RosterTreeView(QWidget *parent): QTreeView(parent)
{
  m_delegate = new Kitty::RosterItemDelegate(this);
  setItemDelegate(m_delegate);
}
