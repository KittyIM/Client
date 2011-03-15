#ifndef CHATTAB_H
#define CHATTAB_H

#include <QtGui/QWidget>

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
      explicit ChatTab(QWidget *parent = 0);
      ~ChatTab();

    private:
      Ui::ChatTab *m_ui;
  };
}

#endif // CHATTAB_H
