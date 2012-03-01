#ifndef ROSTERTREEVIEW_H
#define ROSTERTREEVIEW_H

#include <QtCore/QPointer>
#include <QtGui/QTreeView>

namespace KittySDK
{
	class IContact;
	class IMessage;
}

namespace Kitty
{
	class RosterItemDelegate;
	class ContactWindow;
	class RosterContact;

	class RosterTreeView: public QTreeView
	{
		Q_OBJECT

		public:
			explicit RosterTreeView(QWidget *parent = 0);

		public slots:
			void fixGroups();
			void selectFirst();
			void sendMessage();
			void copyName();
			void copyUid();
			void copyDescription();
			void moveToGroup();
			void showVCard();
			void showHistory();
			void openURL();

		private slots:
			void itemExpanded(const QModelIndex &index);
			void itemCollapsed(const QModelIndex &index);
			void setupBlinking(quint32 msgId, const KittySDK::IMessage &msg);
			void blinkIcon();
			void unblinkIcon(quint32 msgId);

		signals:
			void vCardRequested(KittySDK::IContact *cnt);
			void historyRequested(KittySDK::IContact *cnt);

		protected:
			void mousePressEvent(QMouseEvent *event);
			void mouseDoubleClickEvent(QMouseEvent *event);
			void keyPressEvent(QKeyEvent *event);

		private:
			RosterContact *rosterContact(const QString &account, const QString &uid);

		private:
			QMap<quint32, QTimer*> m_blinkTimers;
	};
}

#endif // ROSTERTREEVIEW_H
