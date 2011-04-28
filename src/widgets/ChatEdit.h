#ifndef CHATEDIT_H
#define CHATEDIT_H

#include <QtGui/QSyntaxHighlighter>
#include <QtGui/QTextEdit>

namespace Kitty
{
  class SpellChecker: public QSyntaxHighlighter
  {
    Q_OBJECT

    public:
      SpellChecker(QTextDocument *parent): QSyntaxHighlighter(parent) { }

      QStringList suggest(const QString &word);

    private:
      void highlightBlock(const QString &text);
  };

  class ChatEdit: public QTextEdit
  {
    Q_OBJECT

    public:
      explicit ChatEdit(QWidget *parent = 0);

    signals:
      void returnPressed();

    public slots:
      void clearHistory();
      void addHistory(const QString &msg);

    private slots:
      void replaceWord();

    protected:
      void keyPressEvent(QKeyEvent *event);
      void resizeEvent(QResizeEvent *event);
      void contextMenuEvent(QContextMenuEvent *event);

    private:
      void updateSize();

    private:
      QStringList m_history;
      int m_historyPos;
      SpellChecker *m_checker;
  };
}

#endif // CHATEDIT_H
