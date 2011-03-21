#include "RosterItemDelegate.h"

#include "RosterItem.h"

#include <QtGui/QApplication>
#include <QtGui/QPainter>

Kitty::RosterItemDelegate::RosterItemDelegate(QObject *parent): QStyledItemDelegate(parent)
{
}

void Kitty::RosterItemDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
  int type = index.data(RosterItem::TypeRole).toInt();
  QString text = index.data(Qt::DisplayRole).toString();
  QString description = index.data(RosterItem::DescriptionRole).toString();
  QPixmap avatar(index.data(RosterItem::AvatarRole).toString());
  if(!avatar.isNull()) {
    avatar = avatar.scaled(32, 32, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
  }

  QRect rect = option.rect;
  QFont font = option.font;

  painter->save();

  if(type == RosterItem::GROUP) {
    if(option.state & QStyle::State_Selected) {
      painter->setBrush(qApp->palette().button().color().darker(110));
      painter->setPen(qApp->palette().midlight().color().darker(110));
   } else {
      painter->setBrush(qApp->palette().button().color());
      painter->setPen(qApp->palette().midlight().color());
    }

    painter->drawRect(option.rect);

    font.setBold(true);
    painter->setFont(font);

    painter->setPen(Qt::black);
    painter->setBrush(Qt::white);

    painter->drawText(rect.translated(20, 0), Qt::AlignVCenter | Qt::TextSingleLine, text);

    QStyleOption opt;
    opt.state = QStyle::State_Enabled;
    opt.rect = QRect(2, rect.top() + 6, 15, 15);

    if(option.state & QStyle::State_Open) {
      qApp->style()->drawPrimitive(QStyle::PE_IndicatorArrowDown, &opt, painter);
    } else {
      qApp->style()->drawPrimitive(QStyle::PE_IndicatorArrowRight, &opt, painter);
    }
  } else {
    if(option.state & QStyle::State_Selected) {
      painter->setBrush(QColor(54, 151, 255));
      painter->setPen(QColor(54, 151, 255));
      painter->drawRect(option.rect);
    }

    font.setBold(false);
    painter->setFont(font);

    painter->setPen(Qt::black);

    int right = 0;
    if(!avatar.isNull()) {
      right = 34;
    }

    if(description.isEmpty()) {
      painter->drawText(rect.translated(20, 0), Qt::AlignVCenter | Qt::TextSingleLine, QFontMetrics(font).elidedText(text, Qt::ElideRight, rect.width() - (20 + right)));
    } else {
      rect = option.rect;
      rect.translate(20, 3);
      rect.setHeight(16);
      painter->drawText(rect, Qt::TextSingleLine, QFontMetrics(font).elidedText(text, Qt::ElideRight, rect.width() - (20 + right)));

      font.setItalic(true);
      font.setPointSize(font.pointSize() - 1);
      painter->setFont(font);

      rect = option.rect;
      rect.translate(20, rect.height() / 2 + 1);
      rect.setHeight(18);
      painter->drawText(rect, Qt::TextSingleLine, QFontMetrics(font).elidedText(description, Qt::ElideRight, rect.width() - (20 + right)));
    }

    if(!avatar.isNull()) {
      rect = option.rect;
      painter->drawPixmap(rect.width() - 34, rect.top() + 2, 32, 32, avatar);
    }
  }

  painter->restore();
}

QSize Kitty::RosterItemDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
  int height = 26;

  if(index.data(RosterItem::TypeRole).toInt() == RosterItem::CONTACT) {
    height = 36;
  }

  return QSize(option.rect.width(), height);
}
