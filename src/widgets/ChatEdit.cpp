#include "ChatEdit.h"
#include "App.h"

#include <QtCore/QTextBoundaryFinder>
#include <QtCore/QDebug>
#include <QtGui/QKeyEvent>

Kitty::SpellChecker::SpellChecker(QTextDocument *parent): QSyntaxHighlighter(parent)
{
  QByteArray dic = QString(qApp->applicationDirPath() + "/data/dictionaries/pl_PL.dic").toLocal8Bit();
  QByteArray aff = QString(qApp->applicationDirPath() + "/data/dictionaries/pl_PL.aff").toLocal8Bit();

  //TODO: move to Core
  m_hunspell = new Hunspell(aff.constData(), dic.constData());
}

Kitty::SpellChecker::~SpellChecker()
{
  delete m_hunspell;
}

void Kitty::SpellChecker::highlightBlock(const QString &text)
{
  QTextCharFormat format;
  format.setUnderlineColor(Qt::red);
  format.setUnderlineStyle(QTextCharFormat::SpellCheckUnderline);

  QTextBoundaryFinder finder(QTextBoundaryFinder::Word, text);
  int start = 0;
  int length = 0;
  QString word = "";

  while(finder.position() < text.length()) {
    if(finder.position() == 0) {
      start = 0;
    } else {
      start = finder.position();
    }

    length = finder.toNextBoundary() - start;
    word = text.mid(start, length);

    if(!m_hunspell->spell(word.toLatin1().constData())) {
      setFormat(start, length, format);
    }
  }
}


Kitty::ChatEdit::ChatEdit(QWidget *parent): QTextEdit(parent)
{
  m_checker = new Kitty::SpellChecker(document());

  updateSize();
}

void Kitty::ChatEdit::updateSize()
{
  int height = document()->size().height() + document()->documentMargin();
  if(height < 200) {
    setFixedHeight(height);
  }
}

void Kitty::ChatEdit::keyPressEvent(QKeyEvent *event)
{
  if(((event->key() == Qt::Key_Enter) || (event->key() == Qt::Key_Return)) && !(event->modifiers() & Qt::ShiftModifier)) {
    emit returnPressed();
  } else {
    QTextEdit::keyPressEvent(event);
  }

  updateSize();
}

void Kitty::ChatEdit::resizeEvent(QResizeEvent *event)
{
  QTextEdit::resizeEvent(event);

  updateSize();
}

