#ifndef CHATTAB_H
#define CHATTAB_H

#include <IProtocol.h>

#include <QtCore/QTimer>
#include <QtGui/QTextCharFormat>
#include <QtGui/QWidget>

class QToolBar;
class QAction;
class QLabel;

namespace KittySDK
{
	class IContact;
	class IChat;
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
			ChatTab(KittySDK::IChat *chat, QWidget *parent = 0);
			~ChatTab();

			KittySDK::IChat *chat() const { return m_chat; }

			void setTypingNotify(bool typing, const int &length);

		signals:
			void tabChanged();

		public slots:
			void updateIcons();
			void setEditFocus();
			void applySettings();
			void appendMessage(KittySDK::IMessage &msg);
			void clearMessages();

		private slots:
			void updateButtons(const QTextCharFormat &format);
			void updateImageMenu();
			void showColorPicker();
			void sendMessage();
			void sendTypingNotify(bool typing, const int &length);
			void sendImageFile();
			void sendImageWindow();
			void sendImageDesktop();
			void sendImageClipboard();
			void sendImageFragment();
			void sendPixmap(const QPixmap &pix);
			void sendImage(const QString &fileName);
			void changeStatus(KittySDK::IProtocol::Status status, QString description);
			void showContactWindow();
			void showHistoryWindow();
			void checkDequeue(quint32 msgId, const KittySDK::IMessage &msg);
			void editFocused();

		protected:
			void changeEvent(QEvent *event);
			void showEvent(QShowEvent *event);

		private:
			Ui::ChatTab *m_ui;
			KittySDK::IChat *m_chat;
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
			QPointer<QLabel> m_typingLabel;
			int m_messageCount;
			QTimer m_cleanTimer;
	};
}

#endif // CHATTAB_H
