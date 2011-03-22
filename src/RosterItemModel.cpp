#include "RosterItemModel.h"

#include "RosterItem.h"

#include <QtCore/QDebug>

Kitty::RosterItemModel::RosterItemModel(QObject *parent): QAbstractItemModel(parent)
{
  qDebug() << "Creating RosterItemModel";

  m_root = new Kitty::RosterItem();

  Kitty::RosterItem *g1 = addGroup("Group 1");
  addContact("Contact 1", g1)->setData("Lorem ipsum dolor sit amet.", Kitty::RosterItem::DescriptionRole);
  addContact("Contact 2", g1);

  Kitty::RosterItem *g2 = addGroup("Group 2");
  addContact("Contact 3", g2);
  addContact("Contact 4", g2);
  addContact("Contact 5", g2);

  Kitty::RosterItem *g3 = addGroup("Group 3");
  addContact("Contact 6", g3);

  addContact("Contact 7");
}

Kitty::RosterItemModel::~RosterItemModel()
{
  delete m_root;
}

Kitty::RosterItem *Kitty::RosterItemModel::addGroup(const QString &name)
{
  Kitty::RosterItem *item = new Kitty::RosterItem(m_root);

  item->setData(Kitty::RosterItem::GROUP, Kitty::RosterItem::TypeRole);
  item->setData(name, Qt::DisplayRole);

  m_root->appendChild(item);

  return item;
}

Kitty::RosterItem *Kitty::RosterItemModel::addContact(const QString &name, Kitty::RosterItem *parent)
{
  if(!parent) {
    parent = m_root;
  }

  Kitty::RosterItem *item = new Kitty::RosterItem(parent);
  item->setData(Kitty::RosterItem::CONTACT, Kitty::RosterItem::TypeRole);
  item->setData(name, Qt::DisplayRole);

  parent->appendChild(item);

  return item;
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
