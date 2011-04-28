#ifndef CHATWEBVIEW_H
#define CHATWEBVIEW_H

#include <QtNetwork/QNetworkRequest>
#include <QtWebKit/QWebView>

namespace KittySDK
{
  class Message;
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

      void appendMessage(const KittySDK::Message &msg, Kitty::ChatTheme *theme = 0);

    public slots:
      void clear();
      void clearTo(bool custom = false, const QString &theme = QString(), const QString &variant = QString());

    private slots:
      void updateScrollbar();
      void showContextMenu(QPoint pos);
      void handleDownload(QNetworkRequest req);

    protected:
      void keyPressEvent(QKeyEvent *event);

    private:
      ChatWebPage *m_page;
  };
}

#endif // CHATWEBVIEW_H
