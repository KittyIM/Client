#ifndef CHATTAB_H
#define CHATTAB_H

#include <QtGui/QWidget>

class QToolBar;
class QAction;

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

    public slots:
      void updateIcons();

    protected:
      void changeEvent(QEvent *event);

    private:
      Ui::ChatTab *m_ui;
      QToolBar *m_toolBar;
      QAction *m_boldAction;
      QAction *m_italicAction;
      QAction *m_underlineAction;
      QAction *m_colorAction;
      QAction *m_smileyAction;
      QAction *m_imageAction;
      QAction *m_fileAction;
      QAction *m_profileAction;
      QAction *m_historyAction;
  };
}

#endif // CHATTAB_H
