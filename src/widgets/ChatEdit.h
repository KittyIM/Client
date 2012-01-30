#ifndef CHATEDIT_H
#define CHATEDIT_H

#include <QtCore/QTimer>
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
			void typingChanged(bool typing, const int &length);
			void pixmapDropped(const QPixmap &pix);

		public slots:
			void clearHistory();
			void addHistory(const QString &msg);

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

		protected:
			void keyPressEvent(QKeyEvent *event);
			void resizeEvent(QResizeEvent *event);
			void contextMenuEvent(QContextMenuEvent *event);
			bool canInsertFromMimeData(const QMimeData *source) const;
			void insertFromMimeData(const QMimeData *source);

		private:
			void updateSize();

		private:
			QStringList m_history;
			int m_historyPos;
			SpellChecker *m_checker;
			QTimer m_typingTimer;
			QTimer m_typingStopTimer;
	};
}

#endif // CHATEDIT_H
