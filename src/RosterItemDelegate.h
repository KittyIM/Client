#ifndef ROSTERITEMDELEGATE_H
#define ROSTERITEMDELEGATE_H

#include <QtGui/QStyledItemDelegate>

namespace Kitty
{
  class RosterTheme;

  class RosterItemDelegate: public QStyledItemDelegate
  {
    Q_OBJECT

    public:
      explicit RosterItemDelegate(QObject *parent = 0);

      void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
      QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const;

    private:
      RosterTheme *m_theme;
  };
}

#endif // ROSTERITEMDELEGATE_H
