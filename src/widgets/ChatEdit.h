#ifndef CHATEDIT_H
#define CHATEDIT_H

#include <QtGui/QTextEdit>
#include <QtGui/QMainWindow>

namespace Kitty
{
  class ChatEdit: public QTextEdit
  {
    Q_OBJECT

    public:
      explicit ChatEdit(QWidget *parent = 0);

    signals:
      void returnPressed();

    private:
      void updateSize();

    protected:
      void keyPressEvent(QKeyEvent *event);
      void resizeEvent(QResizeEvent *event);
  };
}

#endif // CHATEDIT_H
