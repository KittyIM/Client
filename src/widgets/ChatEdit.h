#ifndef CHATEDIT_H
#define CHATEDIT_H

#include "3rdparty/hunspell/hunspell.hxx"

#include <QtGui/QSyntaxHighlighter>
#include <QtGui/QMainWindow>
#include <QtGui/QTextEdit>

class QTextCodec;

namespace Kitty
{
  class SpellChecker: public QSyntaxHighlighter
  {
    Q_OBJECT

    public:
      SpellChecker(QTextDocument *parent);
      ~SpellChecker();

      QStringList suggest(const QString &word);

    private:
      void highlightBlock(const QString &text);

    private:
      Hunspell *m_hunspell;
      QTextCodec *m_codec;
  };

  class ChatEdit: public QTextEdit
  {
    Q_OBJECT

    public:
      explicit ChatEdit(QWidget *parent = 0);

    signals:
      void returnPressed();

    protected:
      void keyPressEvent(QKeyEvent *event);
      void resizeEvent(QResizeEvent *event);
      void contextMenuEvent(QContextMenuEvent *event);

    private slots:
      void replaceWord();

    private:
      void updateSize();

    private:
      SpellChecker *m_checker;
  };
}

#endif // CHATEDIT_H
