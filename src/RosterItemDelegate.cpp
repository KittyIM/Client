#include "RosterItemDelegate.h"

#include "SDK/constants.h"
#include "RosterTheme.h"
#include "RosterItem.h"
#include "Core.h"

#include <QtGui/QApplication>
#include <QtGui/QPainter>

using namespace KittySDK;

Kitty::RosterItemDelegate::RosterItemDelegate(QObject *parent): QStyledItemDelegate(parent)
{
  m_theme = new Kitty::RosterTheme(Kitty::Core::inst()->setting(Settings::S_ROSTER_THEME).toString());
}

void Kitty::RosterItemDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
  int type = index.data(RosterItem::TypeRole).toInt();
  QString text = index.data(Qt::DisplayRole).toString();
  QPixmap icon = index.data(Qt::DecorationRole).value<QPixmap>();

  QRect rect = option.rect;

  painter->save();

  if(type == RosterItem::Group) {
    //int children = index.data(RosterItem::ChildrenRole).toInt();

    if(option.state & QStyle::State_Selected) {
      painter->setBrush(m_theme->selectedGroupBackground());
      painter->setPen(m_theme->selectedGroupForeground());
   } else {
      painter->setBrush(m_theme->groupBackground());
      painter->setPen(m_theme->groupForeground());
    }
    painter->drawRect(option.rect);

    painter->setFont(m_theme->groupFont());
    painter->setPen(m_theme->groupNameColor());
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
    QString description = index.data(RosterItem::DescriptionRole).toString();
    QPixmap avatar(index.data(RosterItem::AvatarRole).toString());

    if(option.state & QStyle::State_Selected) {
      painter->setBrush(m_theme->selectedContactBackground());
      painter->setPen(m_theme->selectedContactForeground());
    } else {
      painter->setBrush(m_theme->contactBackground());
      painter->setPen(m_theme->contactForeground());
    }
    painter->drawRect(option.rect);

    painter->setFont(m_theme->contactFont());
    painter->setPen(m_theme->contactNameColor());

    int right = 0;
    if(!avatar.isNull()) {
      right = 34;
    }

    if(description.isEmpty()) {
      painter->drawText(rect.translated(20, 0), Qt::AlignVCenter | Qt::TextSingleLine, painter->fontMetrics().elidedText(text, Qt::ElideRight, rect.width() - (20 + right)));
    } else {
      rect = option.rect;
      rect.translate(20, 3);
      rect.setHeight(16);
      painter->drawText(rect, Qt::TextSingleLine, painter->fontMetrics().elidedText(text, Qt::ElideRight, rect.width() - (20 + right)));

      painter->setFont(m_theme->descriptionFont());
      painter->setPen(m_theme->contactDescriptionColor());

      rect = option.rect;
      rect.translate(20, rect.height() / 2 + 1);
      rect.setHeight(18);
      painter->drawText(rect, Qt::TextSingleLine, painter->fontMetrics().elidedText(description, Qt::ElideRight, rect.width() - (20 + right)));
    }

    if(!icon.isNull()) {
      rect = option.rect;
      painter->drawPixmap(rect.left() + 2, rect.top() + 10, icon);
    }

    if(!avatar.isNull()) {
      if(avatar.size() != QSize(32, 32)) {
        avatar = avatar.scaled(32, 32, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
      }

      rect = option.rect;
      painter->drawPixmap(rect.width() - 34, rect.top() + 2, 32, 32, avatar);
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
