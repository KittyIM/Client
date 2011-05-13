#ifndef CHATTAB_H
#define CHATTAB_H

#include "SDK/Protocol.h"

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

    private slots:
      void updateButtons();
      void showColorPicker();
      void sendMessage();
      void changeStatus(KittySDK::Protocol::Status status, QString description);

    protected:
      void changeEvent(QEvent *event);

    private:
      Ui::ChatTab *m_ui;
      KittySDK::Chat *m_chat;
      QToolBar *m_toolBar;
      ChatColorPicker *m_colorPicker;
      QAction *m_boldAction;
      QAction *m_italicAction;
      QAction *m_underlineAction;
      QAction *m_strikethroughAction;
  };
}

#endif // CHATTAB_H
