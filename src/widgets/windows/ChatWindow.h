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
	class Core;

	class ChatWindow: public QWidget
	{
		Q_OBJECT

		public:
			explicit ChatWindow(Core *core, QWidget *parent = 0);
			~ChatWindow();

			ChatTab *tabByChat(KittySDK::IChat *chat);
			bool isChatActive(KittySDK::IChat *chat);

			ChatTheme *theme() const { return m_theme; }

		public slots:
			void applySettings();
			ChatTab *startChat(KittySDK::IChat *chat);
			void switchTo(KittySDK::IChat *chat);
			void showChat(KittySDK::IChat *chat);

		private slots:
			void on_tabWidget_tabCloseRequested(int index);
			void on_tabWidget_currentChanged(int index);

		protected:
			void keyPressEvent(QKeyEvent *event);
			void closeEvent(QCloseEvent *event);

		private:
			Ui::ChatWindow *m_ui;
			Core *m_core;
			ChatTheme *m_theme;
	};
}

#endif // CHATWINDOW_H
