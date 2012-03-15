#ifndef HISTORYWINDOW_H
#define HISTORYWINDOW_H

#include <IContact.h>

#include <QtGui/QSortFilterProxyModel>
#include <QtGui/QWidget>

class QTreeWidgetItem;

namespace Ui
{
	class HistoryWindow;
}

namespace Kitty
{
	class Core;

	class ContactProxy: public QSortFilterProxyModel
	{
		Q_OBJECT

		public:
			explicit ContactProxy(QObject *parent = 0);

		protected:
			bool filterAcceptsRow(int row, const QModelIndex &parent) const;
	};

	class HistoryWindow: public QWidget
	{
		Q_OBJECT

		public:
			enum
			{
				ItemFolder = 1,
				ItemAccount,
				ItemContact
			};

			enum
			{
				TypeRole = Qt::UserRole + 1,
				ProtocolRole,
				AccountRole,
				PathNameRole
			};

		public:
			explicit HistoryWindow(Kitty::Core *core, QWidget *parent = 0);
			~HistoryWindow();

		public slots:
			void applySettings();
			void updateIcons();
			void showContact(KittySDK::IContact *contact);

		protected:
			void changeEvent(QEvent *event);

		private slots:
			void loadData();
			void loadChats(const QItemSelection &selected, const QItemSelection &deselected);
			void filterContacts(const QString &filter);
			void on_chatTree_currentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous);
			void updateCurrentChat();
			void toggleFilters(bool checked);
			void on_chatTree_doubleClicked(const QModelIndex &index);
			void refreshChats();

		private:
			QModelIndex findContact(KittySDK::IContact *contact, const QModelIndex &parent = QModelIndex());

		private:
			Ui::HistoryWindow *m_ui;
			Core *m_core;
			class ContactProxy *m_proxy;
	};
}

#endif // HISTORYWINDOW_H
