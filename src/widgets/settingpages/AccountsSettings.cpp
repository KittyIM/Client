#include "AccountsSettings.h"
#include "ui_AccountsSettings.h"

#include "ProtocolManager.h"
#include "AccountManager.h"
#include "SDK/constants.h"
#include "SDK/Account.h"
#include "IconManager.h"
#include "Core.h"

#include <QtCore/QDebug>
#include <QtGui/QMenu>

using namespace KittySDK;

Kitty::AccountsSettings::AccountsSettings(QWidget *parent): KittySDK::SettingPage(parent), m_ui(new Ui::AccountsSettings)
{
  m_ui->setupUi(this);

  connect(Kitty::AccountManager::inst(), SIGNAL(accountAdded()), this, SLOT(refreshAccounts()));

  setIcon(Icons::I_KEY);
}

Kitty::AccountsSettings::~AccountsSettings()
{
  delete m_ui;
}

void Kitty::AccountsSettings::apply()
{
}

void Kitty::AccountsSettings::reset()
{
}

void Kitty::AccountsSettings::refreshAccounts()
{
  foreach(KittySDK::Account *account, Kitty::AccountManager::inst()->accounts()) {
    if(m_ui->treeWidget->findItems(account->uid(), Qt::MatchExactly).count() == 0) {
      QTreeWidgetItem *item = new QTreeWidgetItem();

      item->setIcon(0, Kitty::Core::inst()->icon(account->protocol()->protoInfo()->protoIcon()));
      item->setText(0, account->uid());
      item->setText(1, account->protocol()->protoInfo()->protoName());

      m_ui->treeWidget->addTopLevelItem(item);
    }
  }
}

void Kitty::AccountsSettings::addAccount()
{
  QAction *action = qobject_cast<QAction*>(sender());
  KittySDK::Protocol *proto = Kitty::ProtocolManager::inst()->protocolByName(action->text());
  if(proto) {
    proto->editWindow()->setWindowModality(Qt::ApplicationModal);
    proto->editWindow()->show();
  } else {
    qWarning() << "Invalid protocol" << action->text();
  }
}

void Kitty::AccountsSettings::on_addButton_clicked()
{

  QMenu menu;

  foreach(KittySDK::Protocol *proto, Kitty::ProtocolManager::inst()->protocols()) {
    menu.addAction(Kitty::IconManager::inst()->icon(proto->protoInfo()->protoIcon()), proto->protoInfo()->protoName(), this, SLOT(addAccount()));
  }

  if(menu.isEmpty()) {
    menu.addAction(tr("No protocols available"));
  }

  menu.exec(m_ui->addButton->mapToGlobal(QPoint(0, m_ui->addButton->height())));
}

void Kitty::AccountsSettings::on_treeWidget_currentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous)
{
  m_ui->editButton->setEnabled(current != 0);
  m_ui->deleteButton->setEnabled(current != 0);
}

void Kitty::AccountsSettings::on_editButton_clicked()
{
  QList<QTreeWidgetItem*> list = m_ui->treeWidget->selectedItems();
  if(list.size() > 0) {
    KittySDK::Protocol *proto = Kitty::ProtocolManager::inst()->protocolByName(list.first()->text(1));
    if(proto) {
      KittySDK::Account *acc = Kitty::AccountManager::inst()->account(proto, list.first()->text(0));
      if(acc) {
        proto->editWindow(acc)->setWindowModality(Qt::ApplicationModal);
        proto->editWindow(acc)->show();
      } else {
        qWarning() << "Account not found" << list.first()->text(0) << list.first()->text(1);
      }
    } else {
      qWarning() << "Protocol not found" << list.first()->text(1);
    }
  }
}
