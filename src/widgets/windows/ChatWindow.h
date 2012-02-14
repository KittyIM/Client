#ifndef CHATWINDOW_H
#define CHATWINDOW_H

#include <QtGui/QWidget>

namespace KittySDK
{
	class IChat;
	class IMessage;
}

namespace Ui
{
	class ChatWindow;
}

namespace Kitty
{
	class ChatTheme;
	class ChatTab;

	class ChatWindow: public QWidget
	{
		Q_OBJECT

		public:
			explicit ChatWindow(QWidget *parent = 0);
			~ChatWindow();

			ChatTab *tabByChat(KittySDK::IChat *chat);

			ChatTheme *theme() const { return m_theme; }

		public slots:
			void applySettings();
			ChatTab *startChat(KittySDK::IChat *chat);
			void switchTo(KittySDK::IChat *chat);

		private slots:
			void on_tabWidget_tabCloseRequested(int index);
			void on_tabWidget_currentChanged(int index);

		protected:
			void keyPressEvent(QKeyEvent *event);
			void closeEvent(QCloseEvent *event);

		private:
			Ui::ChatWindow *m_ui;
			ChatTheme *m_theme;
	};
}

#endif // CHATWINDOW_H
