#include "StatusChangeButton.h"

#include "SDK/constants.h"
#include "Core.h"

#include <QtGui/QStyleOptionToolButton>
#include <QtGui/QStylePainter>
#include <QtGui/QMenu>

using namespace Kitty;
using namespace KittySDK;

Kitty::StatusChangeButton::StatusChangeButton(QWidget *parent): QToolButton(parent)
{
  Core *core = Core::inst();
  QMenu *menu = new QMenu(this);

  menu->addAction(core->icon(Icons::I_STATUS_ONLINE), tr("Online"));
  menu->addAction(core->icon(Icons::I_STATUS_AWAY), tr("Away"));
  menu->addAction(core->icon(Icons::I_STATUS_FFC), tr("Free for chat"));
  menu->addAction(core->icon(Icons::I_STATUS_DND), tr("Do not disturb"));
  menu->addAction(core->icon(Icons::I_STATUS_INVIS), tr("Invisible"));
  menu->addAction(core->icon(Icons::I_STATUS_OFFLINE), tr("Offline"));

  setMenu(menu);
  setPopupMode(QToolButton::InstantPopup);
  setIcon(core->icon(Icons::I_STATUS_OFFLINE));
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
