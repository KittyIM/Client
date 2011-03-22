#ifndef ROSTERTHEME_H
#define ROSTERTHEME_H

#include "AbstractTheme.h"

#include <QtGui/QBrush>
#include <QtGui/QFont>
#include <QtGui/QPen>

namespace Kitty
{
  class RosterTheme: public AbstractTheme
  {
    public:
      RosterTheme(const QString &name);

      QBrush groupBackground() const { return m_groupBackground; }
      QBrush selectedGroupBackground() const { return m_selectedGroupBackground; }
      QBrush contactBackground() const { return m_contactBackground; }
      QBrush selectedContactBackground() const { return m_selectedContactBackground; }

      QPen groupForeground() const { return m_groupForeground; }
      QPen selectedGroupForeground() const { return m_selectedGroupForeground; }
      QPen contactForeground() const { return m_contactForeground; }
      QPen selectedContactForeground() const { return m_selectedContactForeground; }

      QColor groupNameColor() const { return m_groupNameColor; }
      QColor contactNameColor() const { return m_contactNameColor; }
      QColor contactDescriptionColor() const { return m_contactDescriptionColor; }

      QFont groupFont() const { return m_groupFont; }
      QFont contactFont() const { return m_contactFont; }
      QFont descriptionFont() const { return m_descriptionFont; }

    private:
      QBrush m_groupBackground;
      QBrush m_selectedGroupBackground;
      QBrush m_contactBackground;
      QBrush m_selectedContactBackground;

      QPen m_groupForeground;
      QPen m_selectedGroupForeground;
      QPen m_contactForeground;
      QPen m_selectedContactForeground;

      QColor m_groupNameColor;
      QColor m_contactNameColor;
      QColor m_contactDescriptionColor;

      QFont m_groupFont;
      QFont m_contactFont;
      QFont m_descriptionFont;
  };
}
#endif // ROSTERTHEME_H
