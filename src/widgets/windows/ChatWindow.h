#ifndef CHATWINDOW_H
#define CHATWINDOW_H

#include <QtGui/QWidget>

namespace KittySDK
{
	class Chat;
	class Message;
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

			ChatTab *tabByChat(KittySDK::Chat *chat);

			Kitty::ChatTheme *theme() const { return m_theme; }

		public slots:
			void applySettings();
			ChatTab *startChat(KittySDK::Chat *chat);
			void switchTo(KittySDK::Chat *chat);

		private slots:
			void on_tabWidget_tabCloseRequested(int index);
			void on_tabWidget_currentChanged(int index);

		protected:
			void keyPressEvent(QKeyEvent *event);
			void closeEvent(QCloseEvent *event);

		private:
			Ui::ChatWindow *m_ui;
			Kitty::ChatTheme *m_theme;
	};
}

#endif // CHATWINDOW_H
