#include "PluginsSettings.h"
#include "ui_PluginsSettings.h"
#include "PluginManager.h"

#include "SDK/constants.h"
#include "Core.h"

#include <QtCore/QFileInfo>
#include <QtCore/QDebug>

using namespace Kitty;
using namespace KittySDK;

Kitty::PluginsSettings::PluginsSettings(QWidget *parent): SettingPage(0, parent), m_ui(new Ui::PluginsSettings)
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
    QTreeWidgetItem *item = new QTreeWidgetItem(m_ui->pluginWidget);

    PluginInfo *info = plugin->plugin()->info();
    if(info) {
      item->setText(0, plugin->plugin()->info()->name());
      item->setText(1, plugin->plugin()->info()->version());
    } else {
      item->setText(0, tr("Unknown"));
      item->setText(1, tr("Unknown"));
    }

    item->setText(2, QFileInfo(plugin->fileName()).fileName());
    item->setText(3, (plugin->isLoaded())?tr("Loaded"):tr("Not loaded"));
  }
}

void Kitty::PluginsSettings::on_pluginWidget_currentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous)
{
  if(current) {
    Plugin *plug = PluginManager::inst()->pluginByFileName(current->text(2));
    if(plug) {
      PluginInfo *info = plug->plugin()->info();
      if(info) {
        m_ui->pluginNameValueLabel->setText(info->name());
        m_ui->pluginAuthorValueLabel->setText(QString("%1 &lt;<a href=\"mailto:%2\">%2</a>&gt;").arg(info->author()).arg(info->email()));
        m_ui->pluginVersionValueLabel->setText(info->version());
        m_ui->pluginWWWValueLabel->setText(QString("<a href=\"%1\">%1</a>").arg(info->www()));
      } else {
        m_ui->pluginNameValueLabel->setText(tr("Unknown"));
        m_ui->pluginAuthorValueLabel->setText(tr("Unknown"));
        m_ui->pluginVersionValueLabel->setText(tr("Unknown"));
        m_ui->pluginWWWValueLabel->setText(tr("Unknown"));
      }
    }

    m_ui->pluginInfoGroupBox->show();
  } else {
    m_ui->pluginInfoGroupBox->hide();
  }
}

void Kitty::PluginsSettings::retranslate()
{
  m_ui->retranslateUi(this);

  for(int i = 0; i < m_ui->pluginWidget->topLevelItemCount(); i++) {
    QTreeWidgetItem *item = m_ui->pluginWidget->topLevelItem(i);
    Plugin *plugin = PluginManager::inst()->pluginByName(item->text(0));
    if(plugin) {
      item->setText(3, (plugin->isLoaded())?tr("Loaded"):tr("Not loaded"));
    }
  }
}
