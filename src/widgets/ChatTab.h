#ifndef CHATTAB_H
#define CHATTAB_H

#include <QtGui/QWidget>

class QToolBar;
class QAction;

namespace KittySDK
{
  class Chat;
}

namespace Ui
{
  class ChatTab;
}

namespace Kitty
{
  class ChatTab: public QWidget
  {
    Q_OBJECT

    public:
      ChatTab(KittySDK::Chat *chat, QWidget *parent = 0);
      ~ChatTab();

      KittySDK::Chat *chat() const { return m_chat; }

    public slots:
      void updateIcons();
      void setEditFocus();
      void applySettings();

    private slots:
      void sendMessage();

    protected:
      void changeEvent(QEvent *event);

    private:
      Ui::ChatTab *m_ui;
      KittySDK::Chat *m_chat;
      QToolBar *m_toolBar;
  };
}

#endif // CHATTAB_H
