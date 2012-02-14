#ifndef CHATWEBVIEW_H
#define CHATWEBVIEW_H

#include <QtCore/QDateTime>
#include <QtNetwork/QNetworkRequest>
#include <QtWebKit/QWebView>

#include <IMessage.h>

namespace KittySDK
{
	class IContact;
}

namespace Kitty
{
	class ChatTheme;

	class ChatWebPage: public QWebPage
	{
		Q_OBJECT

		public:
			explicit ChatWebPage(QObject *parent = 0): QWebPage(parent) { }

		protected:
			bool acceptNavigationRequest(QWebFrame *frame, const QNetworkRequest &request, QWebPage::NavigationType type);
	};

	class ChatWebView: public QWebView
	{
		Q_OBJECT

		public:
			explicit ChatWebView(QWidget *parent = 0);
			~ChatWebView();

			void appendMessage(const KittySDK::IMessage &msg, ChatTheme *theme = 0);

			void setAutoScroll(bool autoScroll) { m_autoScroll = autoScroll; }
			bool autoScroll() const { return m_autoScroll; }

		signals:
			void keyPressed();

		public slots:
			void clear();
			void clearTo(bool custom = false, const QString &theme = QString(), const QString &variant = QString());

		private slots:
			void updateScrollbar();
			void showContextMenu(QPoint pos);
			void handleDownload(QNetworkRequest req);

		protected:
			void keyPressEvent(QKeyEvent *event);
			void mouseReleaseEvent(QMouseEvent *event);

		private:
			QString findImages(const QString &body, ChatTheme *theme);
			QString findYoutubes(const QString &body, ChatTheme *theme);

		private:
			ChatWebPage *m_page;
			int m_imageCount;
			KittySDK::IContact *m_lastFrom;
			QDateTime m_lastTimeStamp;
			KittySDK::IMessage::Direction m_lastDirection;
			bool m_autoScroll;
	};
}

#endif // CHATWEBVIEW_H
