#include "SettingsWindow.h"
#include "ui_SettingsWindow.h"

#include "widgets/settingpages/ChatWindowTabsSettings.h"
#include "widgets/settingpages/ConnectionSettings.h"
#include "widgets/settingpages/ChatWindowSettings.h"
#include "widgets/settingpages/AccountsSettings.h"
#include "widgets/settingpages/DisplaySettings.h"
#include "widgets/settingpages/HistorySettings.h"
#include "widgets/settingpages/PluginsSettings.h"
#include "widgets/settingpages/StartupSettings.h"
#include "widgets/settingpages/SmiliesSettings.h"
#include "widgets/settingpages/RosterSettings.h"
#include "widgets/settingpages/ThemesSettings.h"
#include "widgets/settingpages/MainSettings.h"
#include "widgets/settingpages/UserSettings.h"
#include "IconManager.h"
#include "MainWindow.h"
#include "Core.h"
#include "App.h"

#include <SDKConstants.h>

#include <QtCore/QDebug>
#include <QtCore/QDir>
#include <QtGui/QTreeWidgetItem>
#include <QtGui/QPushButton>

#define qDebug() qDebug() << "[SettingsWindow]"
#define qWarning() qWarning() << "[SettingsWindow]"

namespace Kitty
{

SettingsWindow::SettingsWindow(QWidget *parent): QDialog(parent), m_ui(new Ui::SettingsWindow)
{
	m_ui->setupUi(this);
	m_ui->treeWidget->header()->hideSection(1);

	connect(m_ui->buttonBox->button(QDialogButtonBox::Apply), SIGNAL(clicked()), this, SLOT(applySettings()));
	connect(IconManager::inst(), SIGNAL(iconsUpdated()), this, SLOT(updateIcons()));

	setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);

	qDebug() << "Creating";

	Core *core = Core::inst();

	connect(this, SIGNAL(settingsApplied()), dynamic_cast<App*>(qApp), SLOT(applySettings()));
	connect(this, SIGNAL(settingsApplied()), core->mainWindow(), SLOT(applySettings()));

	restoreGeometry(core->setting(KittySDK::Settings::S_SETTINGSWINDOW_GEOMETRY).toByteArray());

	addDefaultPages();

	updateIcons();
}

SettingsWindow::~SettingsWindow()
{
	Core *core = Core::inst();

	core->setSetting(KittySDK::Settings::S_SETTINGSWINDOW_GEOMETRY, saveGeometry());

	qDeleteAll(m_pages);

	delete m_ui;
}

QTreeWidgetItem *SettingsWindow::itemById(const QString &id)
{
	QList<QTreeWidgetItem*> list = m_ui->treeWidget->findItems(id, Qt::MatchExactly | Qt::MatchRecursive, 1);
	if(list.count() > 0) {
		return list.first();
	}

	qWarning() << "Settings item not found [" << id << "]";

	return 0;
}

KittySDK::ISettingsPage *SettingsWindow::pageById(const QString &id)
{
	foreach(KittySDK::ISettingsPage *page, m_pages) {
		if(page->id() == id) {
			return page;
		}
	}

	return 0;
}

void SettingsWindow::addPage(KittySDK::ISettingsPage *page, const QString &parent)
{
	if(pageById(page->id())) {
		qWarning() << "Page with id" << page->id() << "already exists!";
		return;
	}

	QTreeWidgetItem *child = new QTreeWidgetItem();
	child->setIcon(0, IconManager::inst()->icon(page->icon()));
	child->setText(0, page->windowTitle());
	child->setText(1, page->id());

	if(parent.isEmpty()) {
		m_ui->stackedWidget->addWidget(page);
		m_pages.append(page);
		m_ui->treeWidget->addTopLevelItem(child);
	} else {
		if(QTreeWidgetItem *item = itemById(parent)) {
			m_ui->stackedWidget->addWidget(page);
			m_pages.append(page);
			item->addChild(child);
		} else {
			qWarning() << "Parent doesn't exist [" << parent << "]";
		}
	}
}

void SettingsWindow::updateIcons()
{
	Core *core = Core::inst();

	foreach(KittySDK::ISettingsPage *page, m_pages) {
		if(QTreeWidgetItem *item = itemById(page->id())) {
			item->setIcon(0, core->icon(page->icon()));
		} else {
			qWarning() << "Page doesn't exist [" << page->id() << "]";
		}
	}

	if(DisplaySettings *display = qobject_cast<DisplaySettings*>(pageById(KittySDK::SettingPages::S_DISPLAY))) {
		display->updateIcons();
	}
}

void SettingsWindow::resetSettings()
{
	qDebug() << "Resetting all pages [" << m_pages.count() << "]";
	foreach(KittySDK::ISettingsPage *page, m_pages) {
		page->reset();
	}
}

void SettingsWindow::addDefaultPages()
{
	qDebug() << "Adding default pages";

	addPage(new MainSettings(this));
	addPage(new StartupSettings(this), KittySDK::SettingPages::S_SETTINGS);
	addPage(new ConnectionSettings(this), KittySDK::SettingPages::S_SETTINGS);

	addPage(new UserSettings(this));
	addPage(new AccountsSettings(this), KittySDK::SettingPages::S_USER);

	addPage(new DisplaySettings(this));
	addPage(new ThemesSettings(this), KittySDK::SettingPages::S_DISPLAY);
	addPage(new RosterSettings(this), KittySDK::SettingPages::S_DISPLAY);
	addPage(new HistorySettings(this), KittySDK::SettingPages::S_DISPLAY);
	addPage(new ChatWindowSettings(this), KittySDK::SettingPages::S_DISPLAY);
	addPage(new ChatWindowTabsSettings(this), KittySDK::SettingPages::S_DISPLAY_CHATWINDOW);
	addPage(new SmiliesSettings(this), KittySDK::SettingPages::S_DISPLAY_CHATWINDOW);

	addPage(new PluginsSettings(this));
}

void SettingsWindow::showEvent(QShowEvent *event)
{
	QDialog::showEvent(event);

	resetSettings();

	m_ui->treeWidget->expandAll();
	if(m_ui->treeWidget->topLevelItemCount()) {
		m_ui->treeWidget->setCurrentItem(m_ui->treeWidget->topLevelItem(0));
	}
	m_ui->buttonBox->setFocus();
}

void SettingsWindow::changeEvent(QEvent *event)
{
	if(event->type() == QEvent::LanguageChange) {
		m_ui->retranslateUi(this);

		foreach(KittySDK::ISettingsPage *page, m_pages) {
			page->retranslate();
			if(QTreeWidgetItem *item = itemById(page->id())) {
				item->setText(0, page->windowTitle());
			}
		}

		on_treeWidget_currentItemChanged(m_ui->treeWidget->currentItem(), 0);
	}

	QDialog::changeEvent(event);
}

void SettingsWindow::on_treeWidget_currentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous)
{
	if(current) {
		m_ui->groupBox->setTitle(current->text(0));

		for(int i = 0; i < m_ui->stackedWidget->count(); ++i) {
			if(m_ui->stackedWidget->widget(i)->property("id").toString() == current->text(1)) {
				m_ui->stackedWidget->setCurrentIndex(i);
				return;
			}
		}

		qDebug() << "Page not found" << current->text(1);
	}
}

void SettingsWindow::applySettings()
{
	qDebug() << "Applying all pages [" << m_pages.count() << "]";

	foreach(KittySDK::ISettingsPage *page, m_pages) {
		page->apply();
	}

	emit settingsApplied();
}

void SettingsWindow::on_buttonBox_accepted()
{
	applySettings();
	accept();
}

void SettingsWindow::on_buttonBox_rejected()
{
	reject();
}

}
