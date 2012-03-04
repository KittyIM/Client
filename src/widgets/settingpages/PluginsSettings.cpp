#include "PluginsSettings.h"
#include "ui_PluginsSettings.h"
#include "PluginManager.h"

#include "Core.h"

#include <SDKConstants.h>

#include <QtCore/QFileInfo>
#include <QtCore/QDebug>

namespace Kitty
{

PluginsSettings::PluginsSettings(QWidget *parent): KittySDK::ISettingsPage(0, parent), m_ui(new Ui::PluginsSettings)
{
	m_ui->setupUi(this);

	m_ui->pluginWidget->header()->restoreState(Core::inst()->setting("Kitty.Columns.PluginsSettings", m_ui->pluginWidget->header()->saveState()).toByteArray());

	setIcon(KittySDK::Icons::I_PLUGIN);
	setId(KittySDK::SettingPages::S_PLUGINS);
}

PluginsSettings::~PluginsSettings()
{
	Core::inst()->setSetting("Kitty.Columns.PluginsSettings", m_ui->pluginWidget->header()->saveState());
	delete m_ui;
}

void PluginsSettings::apply()
{
}

void PluginsSettings::reset()
{
	m_ui->pluginInfoGroupBox->hide();
	m_ui->pluginWidget->clear();

	foreach(Plugin *plugin, PluginManager::inst()->plugins()) {
		if(plugin->hasError()) {
			continue;
		}

		QTreeWidgetItem *item = new QTreeWidgetItem(m_ui->pluginWidget);

		KittySDK::IPluginInfo *info = plugin->iplugin()->info();
		if(info) {
			item->setText(0, plugin->iplugin()->info()->name());
			item->setText(1, plugin->iplugin()->info()->version());
		} else {
			item->setText(0, tr("Unknown"));
			item->setText(1, tr("Unknown"));
		}

		item->setText(2, QFileInfo(plugin->fileName()).fileName());
		item->setText(3, (plugin->isLoaded())?tr("Loaded"):tr("Not loaded"));
		item->setText(4, plugin->iplugin()->info()->id());
	}
}

void PluginsSettings::on_pluginWidget_currentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous)
{
	if(current) {
		Plugin *plug = PluginManager::inst()->pluginByFileName(current->text(2));
		if(plug) {
			if(KittySDK::IPluginInfo *info = plug->iplugin()->info()) {
				if(!info->name().isEmpty()) {
					m_ui->pluginNameLabel->show();
					m_ui->pluginNameValueLabel->show();
					m_ui->pluginNameValueLabel->setText(info->name());
				} else {
					m_ui->pluginNameLabel->hide();
					m_ui->pluginNameValueLabel->hide();
				}

				if(!info->author().isEmpty()) {
					m_ui->pluginAuthorLabel->show();
					m_ui->pluginAuthorValueLabel->show();

					QString authorString = info->author();
					if(!info->email().isEmpty()) {
						authorString += QString("&lt;<a href=\"mailto:%1\">%1</a>&gt;").arg(info->email());
					}

					m_ui->pluginAuthorValueLabel->setText(authorString);
				} else {
					m_ui->pluginAuthorLabel->hide();
					m_ui->pluginAuthorValueLabel->hide();
				}

				if(!info->version().isEmpty()) {
					m_ui->pluginVersionLabel->show();
					m_ui->pluginVersionValueLabel->show();
					m_ui->pluginVersionValueLabel->setText(info->version());
				} else {
					m_ui->pluginVersionLabel->hide();
					m_ui->pluginVersionValueLabel->hide();
				}

				if(!info->url().isEmpty()) {
					m_ui->pluginWWWLabel->show();
					m_ui->pluginWWWValueLabel->show();
					m_ui->pluginWWWValueLabel->setText(QString("<a href=\"%1\">%1</a>").arg(info->url()));
				} else {
					m_ui->pluginWWWLabel->hide();
					m_ui->pluginWWWValueLabel->hide();
				}
			} else {
				m_ui->pluginNameLabel->hide();
				m_ui->pluginNameValueLabel->hide();

				m_ui->pluginAuthorLabel->hide();
				m_ui->pluginAuthorValueLabel->hide();

				m_ui->pluginVersionLabel->hide();
				m_ui->pluginVersionValueLabel->hide();

				m_ui->pluginWWWLabel->hide();
				m_ui->pluginWWWValueLabel->hide();
			}
		}

		m_ui->pluginInfoGroupBox->show();
	} else {
		m_ui->pluginInfoGroupBox->hide();
	}
}

void PluginsSettings::retranslate()
{
	m_ui->retranslateUi(this);

	for(int i = 0; i < m_ui->pluginWidget->topLevelItemCount(); ++i) {
		QTreeWidgetItem *item = m_ui->pluginWidget->topLevelItem(i);
		if(Plugin *plugin = PluginManager::inst()->pluginById(item->text(4))) {
			item->setText(0, plugin->iplugin()->info()->name());
			item->setText(3, (plugin->isLoaded())?tr("Loaded"):tr("Not loaded"));
		}
	}
}

}
