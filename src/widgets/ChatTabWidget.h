#ifndef CHATTABWIDGET_H
#define CHATTABWIDGET_H

#include <QtGui/QTabWidget>

namespace KittySDK
{
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
			void showRecentlyClosed();
			void clearRecentlyClosed();
			void restoreClosedTab();

		protected:
			void tabInserted(int index);
			void tabRemoved(int index);
			void mousePressEvent(QMouseEvent *event);
			void changeEvent(QEvent *event);

		private:
			QList<ChatTab*> m_tabs;
			QList<KittySDK::IChat*> m_closedTabs;
			QToolButton *m_closedButton;
	};
}

#endif // CHATTABWIDGET_H
