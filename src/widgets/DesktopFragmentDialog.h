#ifndef DESKTOPFRAGMENTDIALOG_H
#define DESKTOPFRAGMENTDIALOG_H

#include <QtGui/QDialog>

class QToolBar;
class QSlider;

namespace Kitty
{
	class DesktopFragmentDialog : public QDialog
	{
		Q_OBJECT

		public:
			enum Handle
			{
				None = 0,
				Inside,
				TopLeft,
				TopCenter,
				TopRight,
				MiddleLeft,
				MiddleRight,
				BottomLeft,
				BottomCenter,
				BottomRight
			};

			enum Mode
			{
				Moving = 0,
				Painting
			};

		public:
			explicit DesktopFragmentDialog(QWidget *parent = 0);

			QPixmap pixmap() const { return m_pixmap; }

			static QPixmap getPixmap(QWidget *parent = 0);

		public slots:
			void accept();

		signals:
			void closeRequested();

		protected:
			void showEvent(QShowEvent *event);
			void paintEvent(QPaintEvent *event);
			void mousePressEvent(QMouseEvent *event);
			void mouseDoubleClickEvent(QMouseEvent *event);
			void mouseMoveEvent(QMouseEvent *event);
			void mouseReleaseEvent(QMouseEvent *event);

		private slots:
			void setMovingMode();
			void setPaintingMode();
			void setMode(Mode mode);
			void changePenColor();
			void setEraserColor();
			void clearPainting();

		private:
			Handle handleAt(const QPoint &p);

		private:
			QPixmap m_pixmap;
			QPixmap m_desktop;
			QPixmap m_painting;
			QPoint m_startPoint;
			QPoint m_stopPoint;
			QPoint m_dragPoint;
			QPoint m_paintPoint;
			Handle m_dragHandle;
			Mode m_mode;
			QToolBar *m_toolBar;
			QToolBar *m_paintBar;
			QSlider *m_penSlider;
			QAction *m_colorAction;
	};
}

#endif // DESKTOPFRAGMENTDIALOG_H
