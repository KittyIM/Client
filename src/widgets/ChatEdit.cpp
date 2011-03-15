#include "ChatEdit.h"

#include <QtCore/QDebug>
#include <QtGui/QKeyEvent>

Kitty::ChatEdit::ChatEdit(QWidget *parent): QTextEdit(parent)
{
  updateSize();
}

void Kitty::ChatEdit::updateSize()
{
  int height = document()->size().height() + document()->documentMargin();
  if(height < 200) {
    setFixedHeight(height);
  }
}

void Kitty::ChatEdit::keyPressEvent(QKeyEvent *event)
{
  if((event->key() == Qt::Key_Return) && !(event->modifiers() & Qt::ShiftModifier)) {
    emit returnPressed();
  } else {
    QTextEdit::keyPressEvent(event);
  }

  updateSize();
}

void Kitty::ChatEdit::resizeEvent(QResizeEvent *event)
{
  QTextEdit::resizeEvent(event);

  updateSize();
}
