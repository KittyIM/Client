#include "DisplaySettings.h"
#include "ui_DisplaySettings.h"

#include "AccountManager.h"
#include "Core.h"

#include <SDKConstants.h>

#include <QtGui/QToolTip>

namespace Kitty
{

DisplaySettings::DisplaySettings(QWidget *parent):
	KittySDK::ISettingsPage(0, parent),
	m_ui(new Ui::DisplaySettings)
{
	m_ui->setupUi(this);

	connect(AccountManager::inst(), SIGNAL(accountAdded()), SLOT(refreshAccounts()));

	setIcon(KittySDK::Icons::I_PALETTE);
	setId(KittySDK::SettingPages::S_DISPLAY);
}

DisplaySettings::~DisplaySettings()
{
	delete m_ui;
}

void DisplaySettings::apply()
{
	Core *core = Core::inst();

	QString trayProtocol = m_ui->systemTrayAccountComboBox->itemData(m_ui->systemTrayAccountComboBox->currentIndex()).toString();
	QString trayAccount = m_ui->systemTrayAccountComboBox->itemData(m_ui->systemTrayAccountComboBox->currentIndex(), Qt::UserRole + 1).toString();
	if(trayProtocol.isEmpty() || trayAccount.isEmpty()) {
		core->setSetting(KittySDK::Settings::S_TRAYICON_PROTOCOL, QVariant());
		core->setSetting(KittySDK::Settings::S_TRAYICON_ACCOUNT, QVariant());
	} else {
		core->setSetting(KittySDK::Settings::S_TRAYICON_PROTOCOL, trayProtocol);
		core->setSetting(KittySDK::Settings::S_TRAYICON_ACCOUNT, trayAccount);
	}

	core->setSetting(KittySDK::Settings::S_BLINKING_SPEED, m_ui->blinkingSlider->value());
	core->setSetting(KittySDK::Settings::S_MAINWINDOW_CAPTION, m_ui->mainWindowCaptionEdit->text());
	core->setSetting(KittySDK::Settings::S_CHATWINDOW_CAPTION, m_ui->chatWindowCaptionEdit->text());
	core->setSetting(KittySDK::Settings::S_CHATTAB_CAPTION, m_ui->chatTabCaptionEdit->text());
}

void DisplaySettings::reset()
{
	Core *core = Core::inst();

	refreshAccounts();

	m_ui->blinkingSlider->setValue(core->setting(KittySDK::Settings::S_BLINKING_SPEED, 500).toInt());
	m_ui->mainWindowCaptionEdit->setText(core->setting(KittySDK::Settings::S_MAINWINDOW_CAPTION, "KittyIM %version% [%profile%]").toString());
	m_ui->chatWindowCaptionEdit->setText(core->setting(KittySDK::Settings::S_CHATWINDOW_CAPTION, "%display% [%status%] %description%").toString());
	m_ui->chatTabCaptionEdit->setText(core->setting(KittySDK::Settings::S_CHATTAB_CAPTION, "%display%").toString());
}

void DisplaySettings::updateIcons()
{
	m_ui->mainWindowCaptionHelpButton->setIcon(Core::inst()->icon(KittySDK::Icons::I_INFO));
	m_ui->chatWindowCaptionHelpButton->setIcon(Core::inst()->icon(KittySDK::Icons::I_INFO));
	m_ui->chatTabCaptionHelpButton->setIcon(Core::inst()->icon(KittySDK::Icons::I_INFO));
}

void DisplaySettings::refreshAccounts()
{
	Core *core = Core::inst();

	m_ui->systemTrayAccountComboBox->clear();
	m_ui->systemTrayAccountComboBox->addItem(core->icon(KittySDK::Icons::I_KITTY), tr("None, use Kitty's icon"));
	foreach(KittySDK::IAccount *acc, AccountManager::inst()->accounts()) {
		if(KittySDK::IProtocol *proto = acc->protocol()) {
			if(KittySDK::IProtocolInfo *info = proto->protoInfo()) {
					QString text = QString("%1 (%2)").arg(acc->uid()).arg(info->protoName());

				m_ui->systemTrayAccountComboBox->addItem(core->icon(info->protoIcon()), text, info->protoName());
				m_ui->systemTrayAccountComboBox->setItemData(m_ui->systemTrayAccountComboBox->count() - 1, acc->uid(), Qt::UserRole + 1);

				if((core->setting(KittySDK::Settings::S_TRAYICON_ACCOUNT).toString() == acc->uid() && (core->setting(KittySDK::Settings::S_TRAYICON_PROTOCOL).toString() == info->protoName()))) {
					m_ui->systemTrayAccountComboBox->setCurrentIndex(m_ui->systemTrayAccountComboBox->count() - 1);
				}
			}
		}
	}
}

void DisplaySettings::on_mainWindowCaptionHelpButton_clicked()
{
	QToolTip::showText(m_ui->mainWindowCaptionHelpButton->mapToGlobal(QPoint(0, 2)), tr("Available variables:") +
					   "<br><i>%version%</i> - " + tr("Kitty's version") +
					   "<br><i>%profile%</i> - " + tr("name of loaded profile"));
}

void DisplaySettings::on_chatWindowCaptionHelpButton_clicked()
{
	QToolTip::showText(m_ui->chatWindowCaptionHelpButton->mapToGlobal(QPoint(0, 2)), tr("Available variables:") +
					   "<br><i>%display%</i> - " + tr("contact's name") +
					   "<br><i>%nickname%</i> - " + tr("contact's nickname") +
					   "<br><i>%firstname%</i> - " + tr("contact's first name") +
					   "<br><i>%lastname%</i> - " + tr("contact's last name") +
					   "<br><i>%status%</i> - " + tr("contact's status") +
					   "<br><i>%description%</i> - " + tr("contact's status description") +
					   "<br><i>%uid%</i> -" + tr("contact's id") +
					   "<br><i>%sex%</i> -" + tr("contact's sex") +
					   "<br><i>%birthday%</i> -" + tr("contact's birthday") +
					   "<br><i>%phone%</i> -" + tr("contact's phone") +
					   "<br><i>%email%</i> -" + tr("contact's email"));
}

void DisplaySettings::on_chatTabCaptionHelpButton_clicked()
{
	QToolTip::showText(m_ui->chatTabCaptionHelpButton->mapToGlobal(QPoint(0, 2)), tr("Available variables:") +
					   "<br><i>%display%</i> - " + tr("contact's name") +
					   "<br><i>%nickname%</i> - " + tr("contact's nickname") +
					   "<br><i>%firstname%</i> - " + tr("contact's first name") +
					   "<br><i>%lastname%</i> - " + tr("contact's last name") +
					   "<br><i>%status%</i> - " + tr("contact's status") +
					   "<br><i>%description%</i> - " + tr("contact's status description") +
					   "<br><i>%uid%</i> -" + tr("contact's id") +
					   "<br><i>%sex%</i> -" + tr("contact's sex") +
					   "<br><i>%birthday%</i> -" + tr("contact's birthday") +
					   "<br><i>%phone%</i> -" + tr("contact's phone") +
					   "<br><i>%email%</i> -" + tr("contact's email"));

}

void DisplaySettings::retranslate()
{
	m_ui->retranslateUi(this);
}

}
