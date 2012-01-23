#ifndef CHATCOLORPICKER_H
#define CHATCOLORPICKER_H

#include <QtGui/QFrame>

class QPushButton;

namespace Kitty
{
	class ChatColorFrame: public QFrame
	{
		Q_OBJECT

		public:
			explicit ChatColorFrame(const QString &color, QWidget *parent = 0);

		signals:
			void clicked(QColor color);

		protected:
			void enterEvent(QEvent *event);
			void leaveEvent(QEvent *event);
			void mousePressEvent(QMouseEvent *event);
	};

	class ChatColorPicker: public QWidget
	{
		Q_OBJECT

		public:
			explicit ChatColorPicker(QWidget *parent = 0);

			void showAt(const QPoint &pos);

		public slots:
			void retranslate();

		signals:
			void colorSelected(QColor color);

		private slots:
			void customColor();

		private:
			QPushButton *m_moreButton;
	};
}

#endif // CHATCOLORPICKER_H
