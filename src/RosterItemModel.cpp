#include "RosterItemModel.h"

#include "RosterContact.h"
#include "RosterGroup.h"
#include "SDK/Contact.h"

#include <QtCore/QDebug>

Kitty::RosterItemModel::RosterItemModel(QObject *parent): QAbstractItemModel(parent)
{
  qDebug() << "Creating RosterItemModel";

  m_root = new Kitty::RosterItem();
}

Kitty::RosterItemModel::~RosterItemModel()
{
  delete m_root;
}

Kitty::RosterItem *Kitty::RosterItemModel::addGroup(const QString &name)
{
  Kitty::RosterGroup *item = new Kitty::RosterGroup(m_root);

  item->setData(Kitty::RosterItem::Group, Kitty::RosterItem::TypeRole);
  item->setData(name, Qt::DisplayRole);

  m_root->appendChild(item);

  return item;
}

Kitty::RosterItem *Kitty::RosterItemModel::addContact(Kitty::RosterContact *item, Kitty::RosterItem *parent)
{
  if(!parent) {
    parent = m_root;
  }

  parent->appendChild(item);

  return item;
}

Kitty::RosterItem *Kitty::RosterItemModel::groupItem(const QString &name)
{
  if(name.isEmpty()) {
    return m_root;
  } else {
    for(int i = 0; i < m_root->childCount(); i++) {
      Kitty::RosterItem *group = m_root->child(i);
      if(group->data(Qt::DisplayRole).toString() == name) {
        return group;
      }
    }

    return addGroup(name);
  }
}

int Kitty::RosterItemModel::columnCount(const QModelIndex &parent) const
{
  return 1;
}

QVariant Kitty::RosterItemModel::data(const QModelIndex &index, int role) const
{
  if(!index.isValid()) {
    return QVariant();
  }

  Kitty::RosterItem *item = static_cast<Kitty::RosterItem*>(index.internalPointer());

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

  Kitty::RosterItem *parentItem;

  if(!parent.isValid()) {
    parentItem = m_root;
  } else {
    parentItem = static_cast<Kitty::RosterItem*>(parent.internalPointer());
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

  RosterItem *childItem = static_cast<Kitty::RosterItem*>(index.internalPointer());
  RosterItem *parentItem = childItem->parent();

  if(parentItem == m_root) {
    return QModelIndex();
  }

  return createIndex(parentItem->row(), 0, parentItem);
}

int Kitty::RosterItemModel::rowCount(const QModelIndex &parent) const
{
  Kitty::RosterItem *parentItem;
  if(parent.column() > 0) {
    return 0;
  }

  if(!parent.isValid()) {
    parentItem = m_root;
  } else {
    parentItem = static_cast<Kitty::RosterItem*>(parent.internalPointer());
  }

  return parentItem->childCount();
}
