#include "SmiliesSettings.h"
#include "ui_SmiliesSettings.h"

#include "EmoticonManager.h"
#include "EmoticonPack.h"

#include <SDKConstants.h>

#include <QtCore/QFileInfo>
#include <QtCore/QDebug>
#include <QtCore/QDir>
#include <QtGui/QPainter>

namespace Kitty
{

void SmiliesDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
	QStyleOptionViewItemV4 opt = option;
	initStyleOption(&opt, index);

	const QWidget *widget = opt.widget;
	QStyle *style = widget ? widget->style() : QApplication::style();
	style->drawControl(QStyle::CE_ItemViewItem, &opt, painter, widget);

	EmoticonPack pack(index.data().toString());
	QList<Emoticon*> emots = pack.emoticons();

	int offsetX = 20;
	for(int i = 0; i < qMin(emots.count(), 10); ++i) {
		QImage img(emots.at(i)->fileName());

		if(img.height() > 20) {
			img = img.scaledToHeight(20, Qt::SmoothTransformation);
		}

		painter->drawImage(QPoint(opt.rect.x() + offsetX, opt.rect.y() + 20), img);
		offsetX += img.width() + 5;

		if(offsetX >= option.rect.width()) {
			break;
		}
	}
}

QSize SmiliesDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
	return QSize(option.rect.width(), 45);
}

SmiliesSettings::SmiliesSettings(QWidget *parent): KittySDK::ISettingsPage(0, parent), m_ui(new Ui::SmiliesSettings)
{
	m_ui->setupUi(this);
	m_ui->smiliesWidget->setItemDelegate(new SmiliesDelegate(this));

	setIcon(KittySDK::Icons::I_SMILEY);
	setId(KittySDK::SettingPages::S_DISPLAY_CHATWINDOW_SMILIES);
}

SmiliesSettings::~SmiliesSettings()
{
	delete m_ui;
}

void SmiliesSettings::apply()
{
}

void SmiliesSettings::reset()
{
	m_ui->smiliesWidget->clear();

	QDir dir(qApp->applicationDirPath() + "/emoticons");
	if(dir.exists()) {
		QFileInfoList list = dir.entryInfoList(QDir::AllDirs | QDir::NoDotAndDotDot);

		foreach(const QFileInfo &info, list) {
			QTreeWidgetItem *item = new QTreeWidgetItem(m_ui->smiliesWidget);
			item->setTextAlignment(0, Qt::AlignLeft | Qt::AlignTop);
			item->setText(0, info.baseName());
			item->setCheckState(0, Qt::Unchecked);
		}
	}
}

void SmiliesSettings::retranslate()
{
	m_ui->retranslateUi(this);
}

}
