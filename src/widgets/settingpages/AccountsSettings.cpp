#include "AccountsSettings.h"
#include "ui_AccountsSettings.h"

#include "ProtocolManager.h"
#include "AccountManager.h"
#include "SDK/constants.h"
#include "SDK/Account.h"
#include "IconManager.h"
#include "Core.h"

#include <QtCore/QDebug>
#include <QtGui/QDialog>
#include <QtGui/QMenu>

using namespace Kitty;
using namespace KittySDK;

Kitty::AccountsSettings::AccountsSettings(QWidget *parent): KittySDK::SettingPage(0, parent), m_ui(new Ui::AccountsSettings)
{
	m_ui->setupUi(this);

	connect(AccountManager::inst(), SIGNAL(accountAdded()), this, SLOT(refreshAccounts()));

	setIcon(Icons::I_KEY);

	refreshAccounts();
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
	Core *core = Core::inst();

	m_ui->addButton->setIcon(core->icon(Icons::I_ADD));
	m_ui->editButton->setIcon(core->icon(Icons::I_EDIT));
	m_ui->deleteButton->setIcon(core->icon(Icons::I_DELETE));
}

void Kitty::AccountsSettings::refreshAccounts()
{
	foreach(Account *account, AccountManager::inst()->accounts()) {
		if(m_ui->treeWidget->findItems(account->uid(), Qt::MatchExactly).count() == 0) {
			QTreeWidgetItem *item = new QTreeWidgetItem(m_ui->treeWidget);

			item->setIcon(0, Core::inst()->icon(account->protocol()->protoInfo()->protoIcon()));
			item->setText(0, account->uid());
			item->setText(1, account->protocol()->protoInfo()->protoName());
		}
	}
}

void Kitty::AccountsSettings::addAccount()
{
	if(QAction *action = qobject_cast<QAction*>(sender())) {
		if(Protocol *proto = ProtocolManager::inst()->protocolByName(action->text())) {
			if(QDialog *dlg = proto->editDialog()) {
				dlg->exec();

				AccountManager::inst()->save(Core::inst()->profileName());
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

void Kitty::AccountsSettings::on_addButton_clicked()
{

	QMenu menu;

	foreach(Protocol *proto, ProtocolManager::inst()->protocols()) {
		menu.addAction(IconManager::inst()->icon(proto->protoInfo()->protoIcon()), proto->protoInfo()->protoName(), this, SLOT(addAccount()));
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
		if(Protocol *proto = ProtocolManager::inst()->protocolByName(list.first()->text(1))) {
			if(Account *acc = AccountManager::inst()->account(proto, list.first()->text(0))) {
				if(QDialog *dlg = proto->editDialog(acc)) {
					dlg->exec();

					AccountManager::inst()->save(Core::inst()->profileName());
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

void Kitty::AccountsSettings::retranslate()
{
	m_ui->retranslateUi(this);
}
