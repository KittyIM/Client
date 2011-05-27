#include "RosterGroup.h"

Kitty::RosterGroup::RosterGroup(RosterItem *parent): RosterItem(parent)
{
}

QVariant Kitty::RosterGroup::data(int role) const
{
  switch(role) {
    case Qt::ToolTipRole:
    {
      QString tooltip;

      tooltip += QString("<b>%1</b><br>").arg(data(Qt::DisplayRole).toString());
      tooltip += QString("<font size=\"2\"><b>%1:</b> %2</font>").arg(QObject::tr("Contacts")).arg(childCount());

      return tooltip;
    }
    break;

    case RosterItem::ExpandedRole:
      return true;
    break;

    default:
      return RosterItem::data(role);
    break;
  }
}


