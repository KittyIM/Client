#include "StatusChangeButton.h"

#include <QtGui/QStyleOptionToolButton>
#include <QtGui/QStylePainter>
#include <QtGui/QMenu>

using namespace Kitty;

Kitty::StatusChangeButton::StatusChangeButton(QWidget *parent): QToolButton(parent)
{
  QMenu *menu = new QMenu(this);

  menu->addAction(tr("Online"));
  menu->addAction(tr("Away"));
  menu->addAction(tr("Free for chat"));
  menu->addAction(tr("Do not disturb"));
  menu->addAction(tr("Invisible"));
  menu->addAction(tr("Offline"));

  setMenu(menu);
  setPopupMode(QToolButton::InstantPopup);
}

QSize Kitty::StatusChangeButton::sizeHint() const
{
  return QSize(2 + iconSize().width() + 2 + fontMetrics().width(text()) + 24, 20);
}

void Kitty::StatusChangeButton::paintEvent(QPaintEvent *event)
{
  QStylePainter painter(this);

  if(underMouse()) {
    QStyleOptionButton opt;
    opt.initFrom(this);

    if(isDown()) {
      opt.state |= QStyle::State_Sunken;
    }

    painter.drawControl(QStyle::CE_PushButton, opt);
  }

  painter.setPen(palette().color(QPalette::Text));

  QRect textRect = rect();
  textRect.moveLeft(iconSize().width() + 4);
  painter.drawText(textRect, Qt::AlignVCenter | Qt::TextSingleLine, text());

  painter.drawPixmap(QPoint(2, (height() - iconSize().height()) / 2), icon().pixmap(iconSize()));

  QStyleOptionComboBox opt;
  opt.initFrom(this);
  opt.rect = QRect(width() - 15, height() / 2 - 6, 11, 13);
  painter.drawPrimitive(QStyle::PE_IndicatorArrowDown, opt);
}
