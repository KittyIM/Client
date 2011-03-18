#include "ChatEdit.h"
#include "App.h"

#include <QtCore/QTextBoundaryFinder>
#include <QtCore/QTextCodec>
#include <QtCore/QDebug>
#include <QtGui/QTextDocumentFragment>
#include <QtGui/QKeyEvent>
#include <QtGui/QMenu>

Kitty::SpellChecker::SpellChecker(QTextDocument *parent): QSyntaxHighlighter(parent)
{
  QByteArray dic = QString(qApp->applicationDirPath() + "/data/dictionaries/pl_PL.dic").toLocal8Bit();
  QByteArray aff = QString(qApp->applicationDirPath() + "/data/dictionaries/pl_PL.aff").toLocal8Bit();

  //TODO: move to Core
  m_hunspell = new Hunspell(aff.constData(), dic.constData());
  m_codec = QTextCodec::codecForName(m_hunspell->get_dic_encoding());
}

Kitty::SpellChecker::~SpellChecker()
{
  delete m_hunspell;
}

QStringList Kitty::SpellChecker::suggest(const QString &word)
{
  char **words;

  int count = m_hunspell->suggest(&words, m_codec->fromUnicode(word).constData());

  QStringList suggestions;
  for(int i = 0; i < count; i++) {
    suggestions.append(m_codec->toUnicode(words[i]));
  }

  m_hunspell->free_list(&words, count);

  return suggestions;
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

    if(!m_hunspell->spell(m_codec->fromUnicode(word).constData())) {
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

void Kitty::ChatEdit::contextMenuEvent(QContextMenuEvent *event)
{

   QTextCursor cursor = textCursor();
   cursor.setPosition(cursorForPosition(event->pos()).position());
   cursor.select(QTextCursor::WordUnderCursor);
   setTextCursor(cursor);

   QString word = cursor.selection().toPlainText();

   QMenu *menu = QTextEdit::createStandardContextMenu();

   if(!cursor.selection().isEmpty()) {
     menu->insertAction(menu->actions().first(), new QAction("Add to dictionary", this));
     menu->addSeparator();
   }

   QStringList suggestions = m_checker->suggest(word);
   QList<QAction*> suggestActions;
   for(int i = 0; i < suggestions.count(); i++) {
     QAction *suggestedWord = new QAction(this);
     suggestedWord->setText(suggestions.at(i));
     connect(suggestedWord, SIGNAL(triggered()), this, SLOT(replaceWord()));
     suggestActions.append(suggestedWord);
   }
   if(suggestActions.count() > 0) {
     menu->insertActions(menu->actions().first(), suggestActions);
     menu->addSeparator();
   }

   menu->exec(event->globalPos());
}

void Kitty::ChatEdit::replaceWord()
{
  QAction *action = qobject_cast<QAction*>(sender());
  if(action) {
    insertPlainText(action->text());
  }
}

