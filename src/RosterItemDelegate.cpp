#include "RosterItemDelegate.h"

#include "SDK/constants.h"
#include "RosterItem.h"
#include "Core.h"

#include <QtCore/QDebug>
#include <QtGui/QApplication>
#include <QtGui/QPainter>

#define qDebug() qDebug() << "[RosterItemDelegate]"
#define qWarning() qWarning() << "[RosterItemDelegate]"

using namespace KittySDK;

Kitty::RosterItemDelegate::RosterItemDelegate(QObject *parent): QStyledItemDelegate(parent)
{
}

void Kitty::RosterItemDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
  int type = index.data(RosterItem::TypeRole).toInt();

  QStyleOptionViewItemV4 vopt = option;
  initStyleOption(&vopt, index);

  const QWidget *widget = vopt.widget;
  QStyle *style = widget?widget->style():QApplication::style();

  painter->save();

  QRect iconRect = style->subElementRect(QStyle::SE_ItemViewItemDecoration, &vopt, vopt.widget);
  QRect textRect = style->subElementRect(QStyle::SE_ItemViewItemText, &vopt, vopt.widget);

  style->drawPrimitive(QStyle::PE_PanelItemViewItem, &vopt, painter, vopt.widget);

  if(type == RosterItem::Group) {
    QStyleOption opt = option;
    opt.rect = QRect(2, textRect.top() + textRect.height() / 2 - 6, 15, 15);

    if(option.state & QStyle::State_Open) {
      style->drawPrimitive(QStyle::PE_IndicatorArrowDown, &opt, painter);
    } else {
      style->drawPrimitive(QStyle::PE_IndicatorArrowRight, &opt, painter);
    }
  } else {
    vopt.icon.paint(painter, iconRect, vopt.decorationAlignment, QIcon::Normal, QIcon::On);
  }

  QPalette::ColorGroup cg = (vopt.state & QStyle::State_Enabled)?QPalette::Normal:QPalette::Disabled;
  if((cg == QPalette::Normal) && !(vopt.state & QStyle::State_Active)) {
    cg = QPalette::Inactive;
  }

  if(vopt.state & QStyle::State_Selected) {
    painter->setPen(vopt.palette.color(cg, QPalette::HighlightedText));
  } else {
    painter->setPen(vopt.palette.color(cg, QPalette::Text));
  }

  if(type == RosterItem::Group) {
    painter->drawText(textRect.translated(17, 0), Qt::AlignVCenter | Qt::TextSingleLine, vopt.text);
  } else {
    QString description;
    if(Core::inst()->setting(Settings::S_ROSTER_STATUS_DESCRIPTION, true).toBool()) {
      description = index.data(RosterItem::DescriptionRole).toString();
    }

    QPixmap avatar(index.data(RosterItem::AvatarRole).toString());
    if(!avatar.isNull()) {
      if(avatar.size() != QSize(32, 32)) {
        avatar = avatar.scaled(32, 32, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
      }

      textRect.adjust(0, 0, -34, 0);
      painter->drawPixmap(vopt.rect.width() - 34, vopt.rect.top() + 2, 32, 32, avatar);
    }

    if(description.isEmpty()) {
      painter->drawText(textRect, Qt::AlignVCenter | Qt::TextSingleLine, painter->fontMetrics().elidedText(vopt.text, Qt::ElideRight, textRect.width()));
    } else {
      QRect rect = textRect;
      rect.translate(0, 3);
      rect.setHeight(16);
      painter->drawText(rect, Qt::TextSingleLine, painter->fontMetrics().elidedText(vopt.text, Qt::ElideRight, textRect.width()));

      QFont descFont = vopt.font;
      descFont.setPointSize(descFont.pointSize() - 1);
      painter->setFont(descFont);

      rect = textRect;
      rect.translate(0, rect.height() / 2 + 1);
      rect.setHeight(18);
      painter->drawText(rect, Qt::TextSingleLine, painter->fontMetrics().elidedText(description, Qt::ElideRight, textRect.width()));
    }
  }

  painter->restore();
}

QSize Kitty::RosterItemDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
  int height = 26;

  if(index.data(RosterItem::TypeRole).toInt() == RosterItem::Contact) {
    height = 36;
  }

  return QSize(option.rect.width(), height);
}
