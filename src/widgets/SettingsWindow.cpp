#include "SettingsWindow.h"
#include "ui_SettingsWindow.h"

#include "Core.h"
#include "SDK/constants.h"

#include <QtCore/QDebug>
#include <QtGui/QTreeWidgetItem>

using namespace Kitty;
using namespace KittySDK;

SettingsWindow::SettingsWindow(QWidget *parent): QDialog(parent), m_ui(new Ui::SettingsWindow)
{
  m_ui->setupUi(this);
  m_ui->treeWidget->header()->hideSection(1);
  m_ui->treeWidget->expandAll();
  //setWindowFlags(windowFlags() & ~Qt::WindowMinMaxButtonsHint);

  Core *core = Core::inst();

  restoreGeometry(core->setting(Settings::S_SETTINGSWINDOW_GEOMETRY).toByteArray());

  updateIcons();
  //applySettings();
}

SettingsWindow::~SettingsWindow()
{
  Core *core = Core::inst();

  core->setSetting(Settings::S_SETTINGSWINDOW_GEOMETRY, saveGeometry());

  delete m_ui;
}

QTreeWidgetItem *SettingsWindow::itemById(const QString &id)
{
  QList<QTreeWidgetItem*> list = m_ui->treeWidget->findItems(id, Qt::MatchExactly | Qt::MatchRecursive, 1);
  if(list.count() > 0) {
    return list.first();
  } else {
    return 0;
  }
}

void SettingsWindow::updateIcons()
{
  Core *core = Core::inst();

  itemById(SettingPages::S_SETTINGS)->setIcon(0, core->icon(Icons::I_SETTINGS));
  itemById(SettingPages::S_SETTINGS_STARTUP)->setIcon(0, core->icon(Icons::I_BULLET));
  itemById(SettingPages::S_SETTINGS_CONNECTION)->setIcon(0, core->icon(Icons::I_CONNECT));
  itemById(SettingPages::S_USER)->setIcon(0, core->icon(Icons::I_USER));
  itemById(SettingPages::S_USER_ACCOUNTS)->setIcon(0, core->icon(Icons::I_KEY));
  itemById(SettingPages::S_DISPLAY)->setIcon(0, core->icon(Icons::I_PALETTE));
  itemById(SettingPages::S_DISPLAY_THEMES)->setIcon(0, core->icon(Icons::I_BULLET));
  itemById(SettingPages::S_DISPLAY_ROSTER)->setIcon(0, core->icon(Icons::I_BULLET));
  itemById(SettingPages::S_DISPLAY_HISTORY)->setIcon(0, core->icon(Icons::I_HISTORY));
  itemById(SettingPages::S_DISPLAY_CHATWINDOW)->setIcon(0, core->icon(Icons::I_MESSAGE));
  itemById(SettingPages::S_DISPLAY_CHATWINDOW_TABS)->setIcon(0, core->icon(Icons::I_TAB));
  itemById(SettingPages::S_DISPLAY_CHATWINDOW_SMILIES)->setIcon(0, core->icon(Icons::I_SMILIE));
  itemById(SettingPages::S_PLUGINS)->setIcon(0, core->icon(Icons::I_PLUGIN));
}

void Kitty::SettingsWindow::on_treeWidget_currentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous)
{
  if(current) {
    m_ui->groupBox->setTitle(current->text(0));

    for(int i = 0; i < m_ui->stackedWidget->count(); i++) {
      if(m_ui->stackedWidget->widget(i)->property("settingPage").toString() == current->text(1)) {
        m_ui->stackedWidget->setCurrentIndex(i);
        return;
      }
    }

    qDebug() << "Page not found :(";
  }
}
