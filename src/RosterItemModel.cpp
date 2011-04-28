#include "RosterItemModel.h"

#include "RosterContact.h"
#include "RosterGroup.h"
#include "SDK/Contact.h"

#include <QtCore/QDebug>

using namespace Kitty;

Kitty::RosterItemModel::RosterItemModel(QObject *parent): QAbstractItemModel(parent)
{
  qDebug() << "Creating RosterItemModel";

  m_root = new RosterItem();
}

Kitty::RosterItemModel::~RosterItemModel()
{
  delete m_root;
}

RosterItem *Kitty::RosterItemModel::addGroup(const QString &name)
{
  RosterGroup *item = new RosterGroup(m_root);

  item->setData(RosterItem::Group, RosterItem::TypeRole);
  item->setData(name, Qt::DisplayRole);

  m_root->appendChild(item);

  return item;
}

void Kitty::RosterItemModel::removeGroup(RosterItem *group)
{
  beginRemoveRows(group->parent()->index(), group->row(), group->row() + 1);

  group->parent()->removeChild(group);
  delete group;

  endRemoveRows();
}

RosterItem *Kitty::RosterItemModel::addContact(RosterContact *item, RosterItem *parent)
{
  if(!parent) {
    parent = m_root;
  }

  parent->appendChild(item);

  return item;
}

RosterItem *Kitty::RosterItemModel::groupItem(const QString &name)
{
  if(name.isEmpty()) {
    return m_root;
  } else {
    for(int i = 0; i < m_root->childCount(); i++) {
      RosterItem *group = m_root->child(i);
      if(group->data(Qt::DisplayRole).toString() == name) {
        return group;
      }
    }

    return addGroup(name);
  }
}

void Kitty::RosterItemModel::moveToGroup(RosterContact *item, const QString &groupName)
{
  RosterItem *newGroup = groupItem(groupName);
  RosterItem *oldGroup = groupItem(item->contact()->group());

  if(newGroup != oldGroup) {
    oldGroup->removeChild(item);
    newGroup->appendChild(item);

    item->contact()->setGroup(groupName);

    if((oldGroup->childCount() == 0) && (oldGroup != m_root)) {
      removeGroup(oldGroup);
    }
  } else {
    qWarning() << "Trying to move contact to same group";
  }
}

int Kitty::RosterItemModel::columnCount(const QModelIndex &parent) const
{
  return 1;
}

/*Qt::ItemFlags Kitty::RosterItemModel::flags(const QModelIndex &index) const
{
  Qt::ItemFlags basic = Qt::ItemIsEnabled | Qt::ItemIsSelectable;

  RosterItem *item = static_cast<RosterItem*>(index.internalPointer());
  if(item->data(RosterItem::TypeRole) == RosterItem::Contact) {
    return basic | Qt::ItemIsDragEnabled;
  } else {
    return basic | Qt::ItemIsDropEnabled;
  }
}*/

QVariant Kitty::RosterItemModel::data(const QModelIndex &index, int role) const
{
  if(!index.isValid()) {
    return QVariant();
  }

  RosterItem *item = static_cast<RosterItem*>(index.internalPointer());

  if(item->data(RosterItem::TypeRole) == RosterItem::Group) {
    switch(role) {
      case Qt::ToolTipRole:
        return QString("<b>%1</b><br>Items: %2").arg(item->text()).arg(item->childCount());
      break;

      // TODO
      case RosterItem::ExpandedRole:
        return true;
      break;
    }
  }

  return item->data(role);
}

QModelIndex Kitty::RosterItemModel::index(int row, int column, const QModelIndex &parent) const
{
  if(!hasIndex(row, column, parent)) {
    return QModelIndex();
  }

  RosterItem *parentItem;

  if(!parent.isValid()) {
    parentItem = m_root;
  } else {
    parentItem = static_cast<RosterItem*>(parent.internalPointer());
  }

  RosterItem *childItem = parentItem->child(row);
  if(childItem) {
    return createIndex(row, column, childItem);
  } else {
    return QModelIndex();
  }
}

QModelIndex Kitty::RosterItemModel::parent(const QModelIndex &index) const
{
  if(!index.isValid()) {
    return QModelIndex();
  }

  RosterItem *childItem = static_cast<RosterItem*>(index.internalPointer());
  RosterItem *parentItem = childItem->parent();

  if(parentItem == m_root) {
    return QModelIndex();
  }

  return createIndex(parentItem->row(), 0, parentItem);
}

int Kitty::RosterItemModel::rowCount(const QModelIndex &parent) const
{
  RosterItem *parentItem;
  if(parent.column() > 0) {
    return 0;
  }

  if(!parent.isValid()) {
    parentItem = m_root;
  } else {
    parentItem = static_cast<RosterItem*>(parent.internalPointer());
  }

  return parentItem->childCount();
}
