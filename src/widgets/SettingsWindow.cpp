#include "SettingsWindow.h"
#include "ui_SettingsWindow.h"

#include "Core.h"
#include "SDK/constants.h"

#include <QtCore/QDebug>
#include <QtGui/QTreeWidgetItem>

using namespace KittySDK;

Kitty::SettingsWindow::SettingsWindow(QWidget *parent): QDialog(parent), m_ui(new Ui::SettingsWindow)
{
  m_ui->setupUi(this);
  m_ui->treeWidget->header()->hideSection(1);
  m_ui->treeWidget->expandAll();

  connect(m_ui->buttonBox->button(QDialogButtonBox::Apply), SIGNAL(clicked()), this, SLOT(applySettings()));

  setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);

  Core *core = Core::inst();

  restoreGeometry(core->setting(Settings::S_SETTINGSWINDOW_GEOMETRY).toByteArray());

  updateIcons();
}

Kitty::SettingsWindow::~SettingsWindow()
{
  Core *core = Core::inst();

  core->setSetting(Settings::S_SETTINGSWINDOW_GEOMETRY, saveGeometry());

  delete m_ui;
}

QTreeWidgetItem *Kitty::SettingsWindow::itemById(const QString &id)
{
  QList<QTreeWidgetItem*> list = m_ui->treeWidget->findItems(id, Qt::MatchExactly | Qt::MatchRecursive, 1);
  if(list.count() > 0) {
    return list.first();
  } else {
    return 0;
  }
}

void Kitty::SettingsWindow::updateIcons()
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

void Kitty::SettingsWindow::resetSettings()
{
  Core *core = Core::inst();

  // Connection
  m_ui->proxyCheckBox->setChecked(core->setting(Settings::S_PROXY_ENABLED).toBool());
  m_ui->proxyServerEdit->setText(core->setting(Settings::S_PROXY_SERVER).toString());
  m_ui->proxyPortEdit->setValue(core->setting(Settings::S_PROXY_PORT, 8080).toInt());
  m_ui->proxyAuthCheckBox->setChecked(core->setting(Settings::S_PROXY_AUTH).toBool());
  m_ui->proxyUsernameEdit->setText(core->setting(Settings::S_PROXY_USERNAME).toString());
  m_ui->proxyPasswordEdit->setText(core->setting(Settings::S_PROXY_PASSWORD).toString());

  // Display
  m_ui->alwaysOnTopCheckBox->setChecked(core->setting(Settings::S_MAINWINDOW_ALWAYS_ON_TOP).toBool());
  m_ui->autoHideCheckBox->setChecked(core->setting(Settings::S_MAINWINDOW_AUTOHIDE).toBool());
  m_ui->autoHideSlider->setValue(core->setting(Settings::S_MAINWINDOW_AUTOHIDE_DELAY).toInt());
  m_ui->transparencyCheckBox->setChecked(core->setting(Settings::S_MAINWINDOW_TRANSPARENCY).toBool());
  m_ui->transparencySlider->setValue(core->setting(Settings::S_MAINWINDOW_TRANSPARENCY_VALUE, 80).toInt());
  m_ui->dockToEdgesCheckBox->setChecked(core->setting(Settings::S_MAINWINDOW_DOCKING).toBool());
  m_ui->dockDistanceSpinBox->setValue(core->setting(Settings::S_MAINWINDOW_DOCKING_DISTANCE).toInt());
  m_ui->mainWindowCaptionEdit->setText(core->setting(Settings::S_MAINWINDOW_CAPTION, "KittyIM %version% [%profile%]").toString());
  m_ui->chatWindowCaptionEdit->setText(core->setting(Settings::S_CHATWINDOW_CAPTION, "%nick% [%status%] \"%description%\"").toString());
  m_ui->chatTabCaptionEdit->setText(core->setting(Settings::S_CHATTAB_CAPTION, "%name%").toString());

  // Roster
  m_ui->displayRosterDescriptionCheckBox->setChecked(core->setting(Settings::S_ROSTER_STATUS_DESCRIPTION, true).toBool());
  m_ui->displayRosterAvatarsCheckBox->setChecked(core->setting(Settings::S_ROSTER_AVATARS, true).toBool());
  m_ui->hideOfflineContactsCheckBox->setChecked(core->setting(Settings::S_ROSTER_HIDE_OFFLINE).toBool());
  m_ui->rosterGroupsAsTabsCheckBox->setChecked(core->setting(Settings::S_ROSTER_GROUPS_AS_TABS).toBool());
  m_ui->rosterGroupsOnMainCheckBox->setChecked(core->setting(Settings::S_ROSTER_GROUPS_ON_MAIN).toBool());
  m_ui->rosterPhoneIconCheckBox->setChecked(core->setting(Settings::S_ROSTER_PHONE_ICON, true).toBool());
  m_ui->rosterEmailIconCheckBox->setChecked(core->setting(Settings::S_ROSTER_EMAIL_ICON, true).toBool());
  m_ui->rosterTipsCheckBox->setChecked(core->setting(Settings::S_ROSTER_TIPS, true).toBool());
  m_ui->rosterTipDelaySpinBox->setValue(core->setting(Settings::S_ROSTER_TIPS_DELAY, 250).toInt());
  m_ui->displayRosterTipsIPCheckBox->setChecked(core->setting(Settings::S_ROSTER_TIPS_IP).toBool());

  foreach(SettingPage *page, m_pages) {
    page->reset();
  }
}

void Kitty::SettingsWindow::showEvent(QShowEvent *event)
{
  resetSettings();

  m_ui->treeWidget->setCurrentItem(m_ui->treeWidget->topLevelItem(0));
  m_ui->buttonBox->setFocus();

  QDialog::showEvent(event);
}

void Kitty::SettingsWindow::on_treeWidget_currentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous)
{
  if(current) {
    m_ui->groupBox->setTitle(current->text(0));

    for(int i = 0; i < m_ui->stackedWidget->count(); i++) {
      if(m_ui->stackedWidget->widget(i)->property("name").toString() == current->text(1)) {
        m_ui->stackedWidget->setCurrentIndex(i);
        return;
      }
    }

    qDebug() << "Page not found :(";
  }
}

void Kitty::SettingsWindow::applySettings()
{
  Core *core = Core::inst();

  // Connection
  core->setSetting(Settings::S_PROXY_ENABLED, m_ui->proxyCheckBox->isChecked());
  core->setSetting(Settings::S_PROXY_SERVER, m_ui->proxyServerEdit->text());
  core->setSetting(Settings::S_PROXY_PORT, m_ui->proxyPortEdit->value());
  core->setSetting(Settings::S_PROXY_AUTH, m_ui->proxyAuthCheckBox->isChecked());
  core->setSetting(Settings::S_PROXY_USERNAME, m_ui->proxyUsernameEdit->text());
  core->setSetting(Settings::S_PROXY_PASSWORD, m_ui->proxyPasswordEdit->text());

  // Display
  core->setSetting(Settings::S_MAINWINDOW_ALWAYS_ON_TOP, m_ui->alwaysOnTopCheckBox->isChecked());
  core->setSetting(Settings::S_MAINWINDOW_AUTOHIDE, m_ui->autoHideCheckBox->isChecked());
  core->setSetting(Settings::S_MAINWINDOW_AUTOHIDE_DELAY, m_ui->autoHideSlider->value());
  core->setSetting(Settings::S_MAINWINDOW_TRANSPARENCY, m_ui->transparencyCheckBox->isChecked());
  core->setSetting(Settings::S_MAINWINDOW_TRANSPARENCY_VALUE, m_ui->transparencySlider->value());
  core->setSetting(Settings::S_MAINWINDOW_DOCKING, m_ui->dockToEdgesCheckBox->isChecked());
  core->setSetting(Settings::S_MAINWINDOW_DOCKING_DISTANCE, m_ui->dockDistanceSpinBox->value());
  core->setSetting(Settings::S_MAINWINDOW_CAPTION, m_ui->mainWindowCaptionEdit->text());
  core->setSetting(Settings::S_CHATWINDOW_CAPTION, m_ui->chatWindowCaptionEdit->text());
  core->setSetting(Settings::S_CHATTAB_CAPTION, m_ui->chatTabCaptionEdit->text());

  // Roster
  core->setSetting(Settings::S_ROSTER_STATUS_DESCRIPTION, m_ui->displayRosterDescriptionCheckBox->isChecked());
  core->setSetting(Settings::S_ROSTER_AVATARS, m_ui->displayRosterAvatarsCheckBox->isChecked());
  core->setSetting(Settings::S_ROSTER_HIDE_OFFLINE, m_ui->hideOfflineContactsCheckBox->isChecked());
  core->setSetting(Settings::S_ROSTER_GROUPS_AS_TABS, m_ui->rosterGroupsAsTabsCheckBox->isChecked());
  core->setSetting(Settings::S_ROSTER_GROUPS_ON_MAIN, m_ui->rosterGroupsOnMainCheckBox->isChecked());
  core->setSetting(Settings::S_ROSTER_PHONE_ICON, m_ui->rosterPhoneIconCheckBox->isChecked());
  core->setSetting(Settings::S_ROSTER_EMAIL_ICON, m_ui->rosterEmailIconCheckBox->isChecked());
  core->setSetting(Settings::S_ROSTER_TIPS, m_ui->rosterTipsCheckBox->isChecked());
  core->setSetting(Settings::S_ROSTER_TIPS_DELAY, m_ui->rosterTipDelaySpinBox->value());
  core->setSetting(Settings::S_ROSTER_TIPS_IP, m_ui->displayRosterTipsIPCheckBox->isChecked());

  foreach(SettingPage *page, m_pages) {
    page->apply();
  }

  emit settingsApplied();
}

void Kitty::SettingsWindow::on_buttonBox_accepted()
{
  applySettings();
  accept();
}

void Kitty::SettingsWindow::on_buttonBox_rejected()
{
  reject();
}
