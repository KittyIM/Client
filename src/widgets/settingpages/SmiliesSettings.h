#ifndef SMILIESSETTINGS_H
#define SMILIESSETTINGS_H

#include <ISettingsPage.h>

#include <QtGui/QStyledItemDelegate>

namespace Ui
{
	class SmiliesSettings;
}

namespace Kitty
{
	class Core;

	class SmiliesDelegate: public QStyledItemDelegate
	{
		Q_OBJECT

		public:
			SmiliesDelegate(QObject *parent = 0): QStyledItemDelegate(parent) { }

			void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
			QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const;
	};

	class SmiliesSettings: public KittySDK::ISettingsPage
	{
		Q_OBJECT

		public:
			SmiliesSettings(Core *core, QWidget *parent = 0);
			~SmiliesSettings();

			void retranslate();
			void apply();
			void reset();

		private:
			Ui::SmiliesSettings *m_ui;
			Core *m_core;
	};
}

#endif // SMILIESSETTINGS_H
