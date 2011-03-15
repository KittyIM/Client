#include "SettingsWindow.h"
#include "ui_SettingsWindow.h"

#include "settingpages/ChatWindowTabsSettings.h"
#include "settingpages/ConnectionSettings.h"
#include "settingpages/ChatWindowSettings.h"
#include "settingpages/AccountsSettings.h"
#include "settingpages/DisplaySettings.h"
#include "settingpages/HistorySettings.h"
#include "settingpages/PluginsSettings.h"
#include "settingpages/StartupSettings.h"
#include "settingpages/SmiliesSettings.h"
#include "settingpages/RosterSettings.h"
#include "settingpages/ThemesSettings.h"
#include "settingpages/MainSettings.h"
#include "settingpages/UserSettings.h"
#include "SDK/constants.h"
#include "IconManager.h"
#include "Core.h"

#include <QtCore/QDebug>
#include <QtCore/QDir>
#include <QtGui/QTreeWidgetItem>
#include <QtGui/QPushButton>

using namespace KittySDK;

Kitty::SettingsWindow::SettingsWindow(QWidget *parent): QDialog(parent), m_ui(new Ui::SettingsWindow)
{
  m_ui->setupUi(this);
  m_ui->treeWidget->header()->hideSection(1);

  connect(m_ui->buttonBox->button(QDialogButtonBox::Apply), SIGNAL(clicked()), this, SLOT(applySettings()));

  setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);

  Core *core = Core::inst();

  restoreGeometry(core->setting(Settings::S_SETTINGSWINDOW_GEOMETRY).toByteArray());

  addDefaultPages();

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

KittySDK::SettingPage *Kitty::SettingsWindow::pageById(const QString &id)
{
  foreach(KittySDK::SettingPage *page, m_pages) {
    if(page->name() == id) {
      return page;
    }
  }

  return 0;
}

void Kitty::SettingsWindow::addPage(KittySDK::SettingPage *page, const QString &parent)
{
  QTreeWidgetItem *child = new QTreeWidgetItem();
  child->setIcon(0, IconManager::inst()->icon(page->icon()));
  child->setText(0, page->windowTitle());
  child->setText(1, page->name());

  if(parent.isEmpty()) {
    m_ui->stackedWidget->addWidget(page);
    m_pages.append(page);
    m_ui->treeWidget->addTopLevelItem(child);
  } else {
    QTreeWidgetItem *item = itemById(parent);
    if(item) {
      m_ui->stackedWidget->addWidget(page);
      m_pages.append(page);
      item->addChild(child);
    } else {
      qDebug() << "No such parent!";
    }
  }
}

void Kitty::SettingsWindow::updateIcons()
{
  Core *core = Core::inst();

  foreach(SettingPage *page, m_pages) {
    QTreeWidgetItem *item = itemById(page->name());
    if(item) {
      item->setIcon(0, core->icon(page->icon()));
    } else {
      qDebug() << "not found" << page->name();
    }
  }
}

void Kitty::SettingsWindow::resetSettings()
{
  foreach(SettingPage *page, m_pages) {
    page->reset();
  }
}

void Kitty::SettingsWindow::addDefaultPages()
{
  addPage(new MainSettings(this));
  addPage(new StartupSettings(this), SettingPages::S_SETTINGS);
  addPage(new ConnectionSettings(this), SettingPages::S_SETTINGS);

  addPage(new UserSettings(this));
  addPage(new AccountsSettings(this), SettingPages::S_USER);

  addPage(new DisplaySettings(this));
  addPage(new ThemesSettings(this), SettingPages::S_DISPLAY);
  addPage(new RosterSettings(this), SettingPages::S_DISPLAY);
  addPage(new HistorySettings(this), SettingPages::S_DISPLAY);
  addPage(new ChatWindowSettings(this), SettingPages::S_DISPLAY);
  addPage(new ChatWindowTabsSettings(this), SettingPages::S_DISPLAY_CHATWINDOW);
  addPage(new SmiliesSettings(this), SettingPages::S_DISPLAY_CHATWINDOW);

  addPage(new PluginsSettings(this));
}

void Kitty::SettingsWindow::showEvent(QShowEvent *event)
{
  resetSettings();

  m_ui->treeWidget->expandAll();
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
