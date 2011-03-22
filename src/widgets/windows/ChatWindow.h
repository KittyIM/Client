#ifndef CHATWINDOW_H
#define CHATWINDOW_H

#include <QtGui/QWidget>

namespace Ui
{
  class ChatWindow;
}

namespace Kitty
{
  class ChatTab;

  class ChatWindow: public QWidget
  {
    Q_OBJECT

    public:
      explicit ChatWindow(QWidget *parent = 0);
      ~ChatWindow();

    public slots:
      void updateIcons();

    private slots:
      void on_tabWidget_tabCloseRequested(int index);

  private:
      Ui::ChatWindow *m_ui;
      QList<ChatTab*> m_tabs;
  };
}

#endif // CHATWINDOW_H
