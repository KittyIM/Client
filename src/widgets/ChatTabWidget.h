#ifndef CHATTABWIDGET_H
#define CHATTABWIDGET_H

#include <QtCore/QMap>
#include <QtGui/QTabWidget>

namespace KittySDK
{
	class IMessage;
	class IChat;
}

class QToolButton;

namespace Kitty
{
	class ChatTab;

	class ChatTabWidget: public QTabWidget
	{
		Q_OBJECT

		public:
			explicit ChatTabWidget(QWidget *parent = 0);
			~ChatTabWidget();

			ChatTab *tabByChat(KittySDK::IChat *chat);
			int indexByChat(KittySDK::IChat *chat);
			int indexByChatId(const QString &chatId);

			QString createLabel(KittySDK::IChat *chat);
			ChatTab *startChat(KittySDK::IChat *chat);
			void switchTo(KittySDK::IChat *chat);

			void removeTab(int index);

		public slots:
			void setCurrentIndex(int index);
			void applySettings();
			void updateTabBar();
			void updateIcons();

		private slots:
			void changeTab();
			void updateTab(int i);
			void updateTabIcon(int i);
			void updateTabFocus(int i);
			void showRecentlyClosed();
			void clearRecentlyClosed();
			void restoreClosedTab();
			void setupBlinking(quint32 msgId, const KittySDK::IMessage &msg);
			void blinkTab();
			void unblinkIcon(quint32 msgId);

		protected:
			void tabInserted(int index);
			void tabRemoved(int index);
			void mousePressEvent(QMouseEvent *event);
			void changeEvent(QEvent *event);

		private:
			QList<ChatTab*> m_tabs;
			QList<KittySDK::IChat*> m_closedTabs;
			QToolButton *m_closedButton;
			QMap<quint32, QTimer*> m_blinkTimers;
	};
}

#endif // CHATTABWIDGET_H
