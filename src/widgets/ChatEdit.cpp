#include "ChatEdit.h"

#include "../lib/hunspell/hunspell.hxx"
#include "Core.h"
#include "App.h"

#include <SDKConstants.h>

#include <QtCore/QTextBoundaryFinder>
#include <QtCore/QTextCodec>
#include <QtCore/QDebug>
#include <QtGui/QTextDocumentFragment>
#include <QtGui/QKeyEvent>
#include <QtGui/QMenu>

#define qDebug() qDebug() << "[ChatEdit]"
#define qWarning() qWarning() << "[ChatEdit]"

namespace Kitty
{

QStringList SpellChecker::suggest(const QString &word)
{
	Hunspell *hunspell = Core::inst()->hunspell();
	char **words;

	int count = hunspell->suggest(&words, QTextCodec::codecForName(hunspell->get_dic_encoding())->fromUnicode(word).constData());

	QStringList suggestions;
	for(int i = 0; i < count; ++i) {
		suggestions.append(QTextCodec::codecForName(hunspell->get_dic_encoding())->toUnicode(words[i]));
	}

	hunspell->free_list(&words, count);

	return suggestions;
}

void SpellChecker::highlightBlock(const QString &text)
{
	Hunspell *hunspell = Core::inst()->hunspell();

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

		if(!hunspell->spell(QTextCodec::codecForName(hunspell->get_dic_encoding())->fromUnicode(word).constData())) {
			setFormat(start, length, format);
		}
	}
}


ChatEdit::ChatEdit(QWidget *parent): QTextEdit(parent)
{
	if(Core::inst()->setting(KittySDK::Settings::S_CHATWINDOW_SPELLCHECK_ENABLED, false).toBool()) {
		m_checker = new SpellChecker(document());
	}

	connect(this, SIGNAL(textChanged()), SLOT(checkTyping()));
	connect(&m_typingTimer, SIGNAL(timeout()), SLOT(sendTypingNotify()));
	connect(&m_typingStopTimer, SIGNAL(timeout()), SLOT(sendTypingStopped()));

	m_typingTimer.setInterval(2000);
	m_typingStopTimer.setSingleShot(true);
	m_typingStopTimer.setInterval(2000);

	updateSize();
	clearHistory();
}

void ChatEdit::updateSize()
{
	int height = document()->size().height() + document()->documentMargin();
	if(height < 200) {
		setFixedHeight(height);
	}
}

void ChatEdit::keyPressEvent(QKeyEvent *event)
{
	if(((event->key() == Qt::Key_Enter) || (event->key() == Qt::Key_Return)) && !(event->modifiers() & Qt::ShiftModifier)) {
		emit returnPressed();
	} else if(((event->key() == Qt::Key_Up) || (event->key() == Qt::Key_Down))&& (event->modifiers() & Qt::ControlModifier))  {
		if(m_history.size() > 0) {
			if(m_historyPos < m_history.size()) {
				setText(m_history.at(m_historyPos));
				selectAll();
			}

			if(event->key() == Qt::Key_Up) {
				m_historyPos++;
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
		QTextEdit::keyPressEvent(event);
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
	QTextCursor cursor = textCursor();
	cursor.setPosition(cursorForPosition(event->pos()).position());
	cursor.select(QTextCursor::WordUnderCursor);
	setTextCursor(cursor);

	QString word = cursor.selection().toPlainText();

	QMenu *menu = QTextEdit::createStandardContextMenu();
	QAction *actFormatted = menu->addAction(tr("Paste formatted"), this, SLOT(pasteFormatted()));
	menu->insertAction(menu->actions().at(6), actFormatted);

	if(!cursor.selection().isEmpty()) {
		menu->insertAction(menu->actions().first(), new QAction("Add to dictionary", this));
		menu->addSeparator();
	}

	QStringList suggestions = m_checker->suggest(word);
	QList<QAction*> suggestActions;
	for(int i = 0; i < suggestions.count(); ++i) {
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
	}
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

void ChatEdit::pasteFormatted()
{
	setAcceptRichText(true);
	paste();
	setAcceptRichText(false);
}

}
