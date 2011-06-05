#ifndef CHATTABWIDGET_H
#define CHATTABWIDGET_H

#include <QtGui/QTabWidget>

namespace KittySDK
{
  class Chat;
}

namespace Kitty
{
  class ChatTab;

  class ChatTabWidget: public QTabWidget
  {
    Q_OBJECT

    public:
      explicit ChatTabWidget(QWidget *parent = 0);
      ~ChatTabWidget();

      ChatTab *tabByChat(KittySDK::Chat *chat);
      int indexByChat(KittySDK::Chat *chat);

      QString createLabel(KittySDK::Chat *chat);
      ChatTab *startChat(KittySDK::Chat *chat);
      void switchTo(KittySDK::Chat *chat);

    public slots:
      void setCurrentIndex(int index);
      void applySettings();
      void updateTabBar();
      void updateIcons();

    private slots:
      void changeTab();
      void updateTab(int i);

    protected:
      void tabInserted(int index);
      void tabRemoved(int index);
      void mousePressEvent(QMouseEvent *event);

    private:
      QList<ChatTab*> m_tabs;
  };
}

#endif // CHATTABWIDGET_H
