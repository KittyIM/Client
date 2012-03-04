#include "PluginErrorWindow.h"
#include "ui_PluginErrorWindow.h"

#include "PluginManager.h"

#include <QtCore/QDebug>
#include <QtCore/QFileInfo>

namespace Kitty
{

PluginErrorWindow::PluginErrorWindow(QWidget *parent) :
	QDialog(parent),
	m_ui(new Ui::PluginErrorWindow)
{
	m_ui->setupUi(this);
	connect(m_ui->pluginList, SIGNAL(currentItemChanged(QListWidgetItem*,QListWidgetItem*)), SLOT(showInfo(QListWidgetItem*,QListWidgetItem*)));

	foreach(Plugin *plug, PluginManager::inst()->plugins()) {
		if(plug->hasError()) {
			QListWidgetItem *item = new QListWidgetItem(m_ui->pluginList);
			item->setText(QFileInfo(plug->fileName()).fileName());
		}
	}

	m_ui->pluginList->setCurrentRow(0);
}

PluginErrorWindow::~PluginErrorWindow()
{
	delete m_ui;
}

void PluginErrorWindow::showInfo(QListWidgetItem *current, QListWidgetItem *)
{
	m_ui->infoEdit->clear();

	if(current) {
		Plugin *plug = PluginManager::inst()->pluginByFileName(current->text());
		m_ui->infoEdit->setPlainText(plug->error());
	}
}

}
