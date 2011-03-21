#include "RosterItemModel.h"

#include "RosterItem.h"

Kitty::RosterItemModel::RosterItemModel(QObject *parent): QAbstractItemModel(parent)
{
  m_root = new RosterItem();

  RosterItem *friends = new RosterItem(RosterItem::GROUP, "Friends", m_root);
  RosterItem *kill3r = new RosterItem(RosterItem::CONTACT, "Kill3r", friends);
  kill3r->setData("Can I haz description?", RosterItem::DescriptionRole);
  friends->appendChild(kill3r);
  friends->appendChild(new RosterItem(RosterItem::CONTACT, "Dawid", friends));
  friends->appendChild(new RosterItem(RosterItem::CONTACT, "Woltaszffasfasfasfasfafasfsaf", friends));
  m_root->appendChild(friends);

  RosterItem *roommates = new RosterItem(RosterItem::GROUP, "Roommates", m_root);
  roommates->appendChild(new RosterItem(RosterItem::CONTACT, "Raze", roommates));
  roommates->appendChild(new RosterItem(RosterItem::CONTACT, "Oli", roommates));
  m_root->appendChild(roommates);

  RosterItem *family = new RosterItem(RosterItem::GROUP, "Family", m_root);
  family->appendChild(new RosterItem(RosterItem::CONTACT, "Peter", family));
  m_root->appendChild(family);
}

Kitty::RosterItemModel::~RosterItemModel()
{
  delete m_root;
}

int Kitty::RosterItemModel::columnCount(const QModelIndex &parent) const
{
  if(parent.isValid()) {
    return static_cast<Kitty::RosterItem*>(parent.internalPointer())->columnCount();
  } else{
    return m_root->columnCount();
  }
}

QVariant Kitty::RosterItemModel::data(const QModelIndex &index, int role) const
{
  if(!index.isValid()) {
    return QVariant();
  }

  Kitty::RosterItem *item = static_cast<RosterItem*>(index.internalPointer());

  if(role == Qt::ToolTipRole) {
    if(item->data(RosterItem::TypeRole) == RosterItem::GROUP) {
      return QString("<b>%1</b><br>Items: %2").arg(item->text()).arg(item->childCount());
    } else {
      return QString("<b>%1</b><br>Group: %2").arg(item->text()).arg(item->parent()->text());
    }
  }

  return item->data(role);
}

QModelIndex Kitty::RosterItemModel::index(int row, int column, const QModelIndex &parent)
const
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
