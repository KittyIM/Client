#include "ChatEdit.h"

#include "../lib/hunspell/hunspell.hxx"
#include "Core.h"
#include "App.h"

#include <SDKConstants.h>

#include <QtCore/QTextBoundaryFinder>
#include <QtCore/QTextCodec>
#include <QtCore/QDebug>
#include <QtCore/QFile>
#include <QtGui/QTextDocumentFragment>
#include <QtGui/QKeyEvent>
#include <QtGui/QMenu>

#define qDebug() qDebug() << "[ChatEdit]"
#define qWarning() qWarning() << "[ChatEdit]"

namespace Kitty
{

SpellChecker::SpellChecker(QTextDocument *parent):
	QSyntaxHighlighter(parent)
{
	m_hunspell = Core::inst()->hunspell();
	m_codec = QTextCodec::codecForName(m_hunspell->get_dic_encoding());
}

QStringList SpellChecker::suggest(const QString &word)
{
	char **words;

	int count = m_hunspell->suggest(&words, m_codec->fromUnicode(word).constData());

	int limit = Core::inst()->setting(KittySDK::Settings::S_CHATWINDOW_SPELLCHECK_SUGGESTIONS, 7).toInt();
	if(limit) {
		count = qMin(count, limit);
	}

	QStringList suggestions;
	for(int i = 0; i < count; ++i) {
		suggestions.append(m_codec->toUnicode(words[i]));
	}

	m_hunspell->free_list(&words, count);

	return suggestions;
}

bool SpellChecker::spell(const QString &word)
{
	return m_hunspell->spell(m_codec->fromUnicode(word).constData());
}

void SpellChecker::addToDictionary(const QString &word)
{
	m_hunspell->add(m_codec->fromUnicode(word).constData());

	QFile dictFile(Core::inst()->currentProfileDir() + "custom.dic");
	if(dictFile.open(QFile::Append | QIODevice::Text)) {
		dictFile.write(m_codec->fromUnicode(word) + "\n");
		dictFile.close();
	}

	rehighlight();
}

void SpellChecker::highlightBlock(const QString &text)
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


ChatEdit::ChatEdit(QWidget *parent):
	QTextEdit(parent),
	m_checker(0)
{
	if(Core::inst()->setting(KittySDK::Settings::S_CHATWINDOW_SPELLCHECK_ENABLED, false).toBool()) {
		m_checker = new SpellChecker(document());
	}

	connect(this, SIGNAL(textChanged()), SLOT(checkTyping()));
	connect(this, SIGNAL(textChanged()), SLOT(updateSize()));
	connect(&m_typingTimer, SIGNAL(timeout()), SLOT(sendTypingNotify()));
	connect(&m_typingStopTimer, SIGNAL(timeout()), SLOT(sendTypingStopped()));

	m_typingTimer.setInterval(2000);
	m_typingStopTimer.setSingleShot(true);
	m_typingStopTimer.setInterval(2000);

	setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

	updateSize();
	clearHistory();
}

void ChatEdit::updateSize()
{
	int height = document()->size().height();
	height = qMin(height, 200);
	setFixedHeight(height + document()->documentMargin());
}

void ChatEdit::resetCharFormat()
{
	QTextCharFormat newFormat = QTextCharFormat();
	newFormat.setFont(font());

	QTextCursor cursor = textCursor();
	cursor.mergeCharFormat(newFormat);

	setCurrentCharFormat(cursor.charFormat());
}

void ChatEdit::keyPressEvent(QKeyEvent *event)
{
	if(((event->key() == Qt::Key_Enter) || (event->key() == Qt::Key_Return)) && !(event->modifiers() & Qt::ShiftModifier)) {
		emit returnPressed();
	} else if(((event->key() == Qt::Key_Up) || (event->key() == Qt::Key_Down)) && (event->modifiers() & Qt::ControlModifier))  {
		if(m_history.size() > 0) {
			if(m_historyPos < m_history.size()) {
				setText(m_history.at(m_historyPos));
				selectAll();
			}

			if(event->key() == Qt::Key_Up) {
				++m_historyPos;
				if(m_historyPos >= m_history.size()) {
					m_historyPos = 0;
				}
			} else {
				m_historyPos--;
				if(m_historyPos < 0) {
					m_historyPos = m_history.size() - 1;
				}
			}
		}
	} else {
		if(!event->matches(QKeySequence::NextChild) && !event->matches(QKeySequence::PreviousChild)) {
			QTextEdit::keyPressEvent(event);
		}

		event->ignore();
	}

	updateSize();
}

void ChatEdit::resizeEvent(QResizeEvent *event)
{
	QTextEdit::resizeEvent(event);

	updateSize();
}

void ChatEdit::contextMenuEvent(QContextMenuEvent *event)
{
	QMenu *menu = QTextEdit::createStandardContextMenu();

	int spell_actions = 0;

	if(m_checker) {
		QTextCursor cursor = textCursor();
		cursor.setPosition(cursorForPosition(event->pos()).position());
		cursor.select(QTextCursor::WordUnderCursor);
		setTextCursor(cursor);

		QString word = cursor.selection().toPlainText();

		if(!m_checker->spell(word)) {
			if(!cursor.selection().isEmpty()) {
				QAction *addAction = new QAction(tr("Add to dictionary"), this);
				addAction->setProperty("word", word);
				connect(addAction, SIGNAL(triggered()), SLOT(addToDictionary()));

				menu->insertSeparator(menu->actions().first());
				menu->insertAction(menu->actions().first(), addAction);

				spell_actions += 2;
			}

			QStringList suggestions = m_checker->suggest(word);
			QList<QAction*> suggestActions;
			for(int i = 0; i < suggestions.count(); ++i) {
				QAction *suggestedWord = new QAction(this);
				QFont boldFont = suggestedWord->font();
				boldFont.setBold(true);
				suggestedWord->setFont(boldFont);
				suggestedWord->setText(suggestions.at(i));
				connect(suggestedWord, SIGNAL(triggered()), this, SLOT(replaceWord()));
				suggestActions.append(suggestedWord);
			}

			if(suggestActions.count() > 0) {
				menu->insertSeparator(menu->actions().first());
				menu->insertActions(menu->actions().first(), suggestActions);

				spell_actions += suggestions.count() + 1;
			}
		}
	}

	QAction *actFormatted = menu->addAction(tr("Paste formatted"), this, SLOT(pasteFormatted()));
	menu->insertAction(menu->actions().at(6 + spell_actions), actFormatted);

	menu->exec(event->globalPos());

	delete menu;
}

bool ChatEdit::canInsertFromMimeData(const QMimeData *source) const
{
	if(source->hasImage()) {
		return true;
	} else {
		return QTextEdit::canInsertFromMimeData(source);
	}
}

void ChatEdit::insertFromMimeData(const QMimeData *source)
{
	if(source->hasImage()) {
		QPixmap pixmap = qvariant_cast<QPixmap>(source->imageData());
		emit pixmapDropped(pixmap);
	} else {
		QTextEdit::insertFromMimeData(source);
	}
}

void ChatEdit::focusInEvent(QFocusEvent *event)
{
	QTextEdit::focusInEvent(event);

	emit focusedIn();
}

void ChatEdit::clearHistory()
{
	m_history.clear();
	m_historyPos = 0;
}

void ChatEdit::addHistory(const QString &msg)
{
	m_history.prepend(msg);
	m_historyPos = 0;

	if(m_history.size() > 25) {
		m_history.removeLast();
	}
}

void ChatEdit::replaceWord()
{
	if(QAction *action = qobject_cast<QAction*>(sender())) {
		insertPlainText(action->text());
	}
}

void ChatEdit::boldText()
{
	QTextCharFormat fmt = currentCharFormat();

	if(fmt.fontWeight() == QFont::Bold) {
		fmt.setFontWeight(QFont::Normal);
	} else {
		fmt.setFontWeight(QFont::Bold);
	}

	setCurrentCharFormat(fmt);
}

void ChatEdit::italicText()
{
	QTextCharFormat fmt = currentCharFormat();
	fmt.setFontItalic(!fmt.fontItalic());
	setCurrentCharFormat(fmt);
}

void ChatEdit::underlineText()
{
	QTextCharFormat fmt = currentCharFormat();
	fmt.setFontUnderline(!fmt.fontUnderline());
	setCurrentCharFormat(fmt);
}

void ChatEdit::colorText(QColor color)
{
	QTextCharFormat fmt = currentCharFormat();
	fmt.setForeground(color);
	setCurrentCharFormat(fmt);
}

void ChatEdit::checkTyping()
{
	if(!m_typingTimer.isActive()) {
		sendTypingNotify();

		m_typingTimer.start();
	}

	m_typingStopTimer.stop();
	m_typingStopTimer.start();
}

void ChatEdit::sendTypingNotify()
{
	emit typingChanged(true, toPlainText().length());
}

void ChatEdit::sendTypingStopped()
{
	m_typingTimer.stop();

	emit typingChanged(false, toPlainText().length());
}

void ChatEdit::addToDictionary()
{
	if(QAction *action = qobject_cast<QAction*>(sender())) {
		if(m_checker) {
			m_checker->addToDictionary(action->property("word").toString());
		}
	}
}

void ChatEdit::pasteFormatted()
{
	setAcceptRichText(true);
	paste();
	setAcceptRichText(false);
}


}
