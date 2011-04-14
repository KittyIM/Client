#include "PluginsSettings.h"
#include "ui_PluginsSettings.h"
#include "PluginManager.h"

#include "SDK/constants.h"
#include "Core.h"

#include <QtCore/QFileInfo>
#include <QtCore/QDebug>

using namespace KittySDK;

Kitty::PluginsSettings::PluginsSettings(QWidget *parent): KittySDK::SettingPage(parent), m_ui(new Ui::PluginsSettings)
{
  m_ui->setupUi(this);

  m_ui->pluginWidget->header()->restoreState(Core::inst()->setting("Kitty.Columns.PluginsSettings", m_ui->pluginWidget->header()->saveState()).toByteArray());

  setIcon(Icons::I_PLUGIN);
}

Kitty::PluginsSettings::~PluginsSettings()
{
  Core::inst()->setSetting("Kitty.Columns.PluginsSettings", m_ui->pluginWidget->header()->saveState());
  delete m_ui;
}

void Kitty::PluginsSettings::apply()
{
}

void Kitty::PluginsSettings::reset()
{
  m_ui->pluginInfoGroupBox->hide();
  m_ui->pluginWidget->clear();

  foreach(Plugin *plugin, PluginManager::inst()->plugins()) {
    QTreeWidgetItem *item = new QTreeWidgetItem();

    item->setText(0, plugin->plugin()->info()->name());
    item->setText(1, plugin->plugin()->info()->version());
    item->setText(2, QFileInfo(plugin->fileName()).fileName());
    item->setText(3, (plugin->isLoaded())?tr("Loaded"):tr("Not loaded"));

    m_ui->pluginWidget->addTopLevelItem(item);
  }
}

void Kitty::PluginsSettings::on_pluginWidget_currentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous)
{
  if(current) {
    Plugin *plug = PluginManager::inst()->pluginByName(current->text(0));
    if(plug) {
      KittySDK::PluginInfo *info = plug->plugin()->info();
      m_ui->pluginNameValueLabel->setText(info->name());
      m_ui->pluginAuthorValueLabel->setText(QString("%1 &lt;<a href=\"mailto:%2\">%2</a>&gt;").arg(info->author()).arg(info->email()));
      m_ui->pluginVersionValueLabel->setText(info->version());
      m_ui->pluginWWWValueLabel->setText(QString("<a href=\"%1\">%1</a>").arg(info->www()));
    }

    m_ui->pluginInfoGroupBox->show();
  } else {
    m_ui->pluginInfoGroupBox->hide();
  }
}
