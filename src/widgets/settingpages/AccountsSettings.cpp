#include "AccountsSettings.h"
#include "ui_AccountsSettings.h"

#include "ProtocolManager.h"
#include "AccountManager.h"
#include "IconManager.h"
#include "Core.h"

#include <SDKConstants.h>
#include <IAccount.h>

#include <QtCore/QDebug>
#include <QtGui/QDialog>
#include <QtGui/QMenu>

namespace Kitty
{

AccountsSettings::AccountsSettings(Core *core, QWidget *parent):
	KittySDK::ISettingsPage(0, parent),
	m_ui(new Ui::AccountsSettings),
	m_core(core)
{
	m_ui->setupUi(this);

	connect(m_core->accountManager(), SIGNAL(accountAdded()), SLOT(refreshAccounts()));
	connect(m_core->accountManager(), SIGNAL(accountStatusChanged(KittySDK::IAccount*,KittySDK::IProtocol::Status,QString)), SLOT(refreshAccount(KittySDK::IAccount*)));

	setIcon(KittySDK::Icons::I_KEY);
	setId(KittySDK::SettingPages::S_USER_ACCOUNTS);

	refreshAccounts();
}

AccountsSettings::~AccountsSettings()
{
	delete m_ui;
}

void AccountsSettings::apply()
{
}

void AccountsSettings::reset()
{
	m_ui->addButton->setIcon(m_core->icon(KittySDK::Icons::I_ADD));
	m_ui->editButton->setIcon(m_core->icon(KittySDK::Icons::I_EDIT));
	m_ui->deleteButton->setIcon(m_core->icon(KittySDK::Icons::I_DELETE));
}

void AccountsSettings::refreshAccounts()
{
	foreach(KittySDK::IAccount *account, m_core->accountManager()->accounts()) {
		refreshAccount(account);
	}
}

void AccountsSettings::refreshAccount(KittySDK::IAccount *account)
{
	QList<QTreeWidgetItem*> items = m_ui->treeWidget->findItems(account->uid(), Qt::MatchExactly);
	QTreeWidgetItem *item = 0;

	if(items.count()) {
		item = items.first();
	} else {
		item = new QTreeWidgetItem(m_ui->treeWidget);
	}

	if(KittySDK::IProtocol *proto = account->protocol()) {
		if(KittySDK::IProtocolInfo *info = proto->protoInfo()) {
			item->setIcon(0, m_core->icon(info->protoIcon()));
			item->setText(0, account->uid());
			item->setText(1, info->protoName());
			item->setText(2, m_core->statusToString(account->status()));
		}
	}
}

void AccountsSettings::addAccount()
{
	if(QAction *action = qobject_cast<QAction*>(sender())) {
		if(KittySDK::IProtocol *proto = m_core->protocolManager()->protocolByName(action->text())) {
			if(QDialog *dlg = proto->editDialog()) {
				dlg->exec();

				m_core->accountManager()->save(m_core->profileName());
			} else {
				qWarning() << "editDialog() returned 0";
			}
		} else {
			qWarning() << "Invalid protocol" << action->text();
		}
	} else {
		qWarning() << "QAction cast failed";
	}
}

void AccountsSettings::on_addButton_clicked()
{

	QMenu menu;

	foreach(KittySDK::IProtocol *proto, m_core->protocolManager()->protocols()) {
		menu.addAction(m_core->iconManager()->icon(proto->protoInfo()->protoIcon()), proto->protoInfo()->protoName(), this, SLOT(addAccount()));
	}

	if(menu.isEmpty()) {
		menu.addAction(tr("No protocols available"));
	}

	menu.exec(m_ui->addButton->mapToGlobal(QPoint(0, m_ui->addButton->height())));
}

void AccountsSettings::on_treeWidget_currentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous)
{
	m_ui->editButton->setEnabled(current != 0);
	m_ui->deleteButton->setEnabled(current != 0);
}

void AccountsSettings::on_editButton_clicked()
{
	QList<QTreeWidgetItem*> list = m_ui->treeWidget->selectedItems();
	if(list.size() > 0) {
		if(KittySDK::IProtocol *proto = m_core->protocolManager()->protocolByName(list.first()->text(1))) {
			if(KittySDK::IAccount *acc = m_core->accountManager()->account(proto, list.first()->text(0))) {
				if(QDialog *dlg = proto->editDialog(acc)) {
					dlg->exec();

					m_core->accountManager()->save(m_core->profileName());
				} else {
					qWarning() << "editDialog() returned 0";
				}
			} else {
				qWarning() << "Account not found" << list.first()->text(0) << list.first()->text(1);
			}
		} else {
			qWarning() << "Protocol not found" << list.first()->text(1);
		}
	}
}

void AccountsSettings::retranslate()
{
	m_ui->retranslateUi(this);
}

}
