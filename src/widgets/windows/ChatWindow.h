#ifndef CHATWINDOW_H
#define CHATWINDOW_H

#include <QtGui/QWidget>

namespace KittySDK
{
  class Chat;
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

      Kitty::ChatTheme *theme() const { return m_theme; }

      QString createLabel(KittySDK::Chat *chat);

    public slots:
      void applySettings();
      void updateIcons();
      void startChat(KittySDK::Chat *chat);
      void switchTo(KittySDK::Chat *chat);

    private slots:
      void on_tabWidget_tabCloseRequested(int index);

    protected:
      void keyPressEvent(QKeyEvent *event);
      void closeEvent(QCloseEvent *event);

    private:
      Ui::ChatWindow *m_ui;
      QList<ChatTab*> m_tabs;
      Kitty::ChatTheme *m_theme;
  };
}

#endif // CHATWINDOW_H
