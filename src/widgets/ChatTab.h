#ifndef CHATTAB_H
#define CHATTAB_H

#include "SDK/Protocol.h"

#include <QtCore/QTimer>
#include <QtGui/QWidget>

class QToolBar;
class QAction;

namespace KittySDK
{
	class Contact;
	class Chat;
}

namespace Ui
{
	class ChatTab;
}

namespace Kitty
{
	class ChatColorPicker;

	class ChatTab: public QWidget
	{
		Q_OBJECT

		public:
			ChatTab(KittySDK::Chat *chat, QWidget *parent = 0);
			~ChatTab();

			KittySDK::Chat *chat() const { return m_chat; }

		signals:
			void tabChanged();

		public slots:
			void updateIcons();
			void setEditFocus();
			void applySettings();
			void appendMessage(KittySDK::Message &msg);
			void clearMessages();

		private slots:
			void updateButtons();
			void showColorPicker();
			void sendMessage();
			void changeStatus(KittySDK::Protocol::Status status, QString description);
			void showContactWindow();
			void showHistoryWindow();

		protected:
			void changeEvent(QEvent *event);

		private:
			Ui::ChatTab *m_ui;
			KittySDK::Chat *m_chat;
			QToolBar *m_toolBar;
			ChatColorPicker *m_colorPicker;
			QPointer<QAction> m_boldAction;
			QPointer<QAction> m_italicAction;
			QPointer<QAction> m_underlineAction;
			QPointer<QAction> m_strikethroughAction;
			QPointer<QAction> m_colorAction;
			QPointer<QAction> m_smiliesAction;
			QPointer<QAction> m_imageAction;
			QPointer<QAction> m_fileAction;
			QPointer<QAction> m_profileAction;
			QPointer<QAction> m_historyAction;
			int m_messageCount;
			QTimer m_cleanTimer;
	};
}

#endif // CHATTAB_H
