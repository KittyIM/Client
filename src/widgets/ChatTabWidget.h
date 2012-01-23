#ifndef CHATTABWIDGET_H
#define CHATTABWIDGET_H

#include <QtGui/QTabWidget>

namespace KittySDK
{
	class Chat;
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

			ChatTab *tabByChat(KittySDK::Chat *chat);
			int indexByChat(KittySDK::Chat *chat);

			QString createLabel(KittySDK::Chat *chat);
			ChatTab *startChat(KittySDK::Chat *chat);
			void switchTo(KittySDK::Chat *chat);

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
			QList<KittySDK::Chat*> m_closedTabs;
			QToolButton *m_closedButton;
	};
}

#endif // CHATTABWIDGET_H
