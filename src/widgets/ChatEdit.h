#ifndef CHATEDIT_H
#define CHATEDIT_H

#include <QtCore/QTimer>
#include <QtGui/QSyntaxHighlighter>
#include <QtGui/QTextEdit>

class QTextCodec;
class Hunspell;

namespace Kitty
{
	class SpellChecker: public QSyntaxHighlighter
	{
		Q_OBJECT

		public:
			SpellChecker(QTextDocument *parent);

			QStringList suggest(const QString &word);
			bool spell(const QString &word);
			void addToDictionary(const QString &word);

		private:
			void highlightBlock(const QString &text);
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
			void focusedIn();
			void typingChanged(bool typing, const int &length);
			void pixmapDropped(const QPixmap &pix);

		public slots:
			void clearHistory();
			void addHistory(const QString &msg);
			void updateSize();
			void resetCharFormat();

		private slots:
			void pasteFormatted();
			void replaceWord();
			void boldText();
			void italicText();
			void underlineText();
			void colorText(QColor color);
			void checkTyping();
			void sendTypingNotify();
			void sendTypingStopped();
			void addToDictionary();

		protected:
			void keyPressEvent(QKeyEvent *event);
			void resizeEvent(QResizeEvent *event);
			void contextMenuEvent(QContextMenuEvent *event);
			bool canInsertFromMimeData(const QMimeData *source) const;
			void insertFromMimeData(const QMimeData *source);
			void focusInEvent(QFocusEvent *event);

		private:
			QStringList m_history;
			int m_historyPos;
			SpellChecker *m_checker;
			QTimer m_typingTimer;
			QTimer m_typingStopTimer;
	};
}

#endif // CHATEDIT_H
