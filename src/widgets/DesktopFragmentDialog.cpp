#include "DesktopFragmentDialog.h"

#include "SDK/constants.h"
#include "Core.h"

#include <QtCore/QDebug>
#include <QtGui/QDesktopWidget>
#include <QtGui/QApplication>
#include <QtGui/QColorDialog>
#include <QtGui/QMouseEvent>
#include <QtGui/QPainter>
#include <QtGui/QToolBar>
#include <QtGui/QToolTip>
#include <QtGui/QAction>
#include <QtGui/QBitmap>
#include <QtGui/QSlider>
#include <QtGui/QLabel>

#define qDebug() qDebug() << "[DesktopFragmentDialog]"
#define qWarning() qWarning() << "[DesktopFragmentDialog]"

using namespace Kitty;
using namespace KittySDK;

bool pointIsNull(const QPoint &p)
{
	return (p.x() == -1) || (p.y() == -1);
}

DesktopFragmentDialog::DesktopFragmentDialog(QWidget *parent):
	QDialog(parent),
	m_startPoint(QPoint(-1, -1)),
	m_stopPoint(QPoint(-1, -1)),
	m_dragPoint(QPoint(-1, -1)),
	m_dragHandle(None),
	m_mode(Moving)
{
	setMouseTracking(true);
	setWindowState(Qt::WindowFullScreen);

	m_toolBar = new QToolBar(this);
	m_toolBar->setAutoFillBackground(true);
	m_toolBar->setIconSize(QSize(16, 16));

	QAction *moveAction = new QAction(m_toolBar);
	moveAction->setIcon(Core::inst()->icon(Icons::I_CURSOR));
	moveAction->setText(tr("Moving/Resizing"));
	connect(moveAction, SIGNAL(triggered()), SLOT(setMovingMode()));
	moveAction->setCheckable(true);
	m_toolBar->addAction(moveAction);

	QAction *paintAction = new QAction(m_toolBar);
	paintAction->setIcon(Core::inst()->icon(Icons::I_PAINTBRUSH));
	paintAction->setText(tr("Painting"));
	connect(paintAction, SIGNAL(triggered()), SLOT(setPaintingMode()));
	paintAction->setCheckable(true);
	m_toolBar->addAction(paintAction);

	m_toolBar->addSeparator();
	m_toolBar->addAction(Core::inst()->icon(Icons::I_QUIT), tr("Cancel"), this, SLOT(reject()));
	m_toolBar->addAction(Core::inst()->icon(Icons::I_ADD), tr("Accept"), this, SLOT(accept()));
	m_toolBar->hide();

	m_paintBar = new QToolBar(this);
	m_paintBar->setAutoFillBackground(true);
	m_paintBar->setIconSize(QSize(16, 16));
	m_paintBar->addWidget(new QLabel(tr("Pen") + ": ", m_paintBar));

	m_penSlider = new QSlider(Qt::Horizontal, m_paintBar);
	m_penSlider->setMinimum(1);
	m_penSlider->setMaximum(15);
	m_paintBar->addWidget(m_penSlider);

	QPixmap icon(16, 16);
	icon.fill(Qt::red);

	m_colorAction = new QAction(m_paintBar);
	m_colorAction->setIcon(icon);
	m_colorAction->setText(tr("Painting color"));
	m_colorAction->setData(QColor(Qt::red));
	connect(m_colorAction, SIGNAL(triggered()), SLOT(changePenColor()));
	m_paintBar->addAction(m_colorAction);

	QAction *eraserAction = new QAction(m_paintBar);
	eraserAction->setIcon(Core::inst()->icon(Icons::I_ERASE));
	eraserAction->setText(tr("Eraser"));
	connect(eraserAction, SIGNAL(triggered()), SLOT(setEraserColor()));
	m_paintBar->addAction(eraserAction);

	QAction *clearAction = new QAction(m_paintBar);
	clearAction->setIcon(Core::inst()->icon(Icons::I_DELETE));
	clearAction->setText(tr("Clear"));
	connect(clearAction, SIGNAL(triggered()), SLOT(clearPainting()));
	m_paintBar->addAction(clearAction);

	m_paintBar->hide();

	setMovingMode();
}

QPixmap DesktopFragmentDialog::getPixmap(QWidget *parent)
{
	DesktopFragmentDialog dialog(parent);

	if(dialog.exec() == QDialog::Accepted) {
		return dialog.pixmap();
	} else {
		return QPixmap();
	}
}

void DesktopFragmentDialog::accept()
{
	m_pixmap = m_desktop.copy(QRect(m_startPoint, m_stopPoint));

	if(!m_painting.isNull()) {
		QPainter p(&m_pixmap);
		p.drawPixmap(0, 0, m_painting);
		p.end();
	}

	QDialog::accept();
}

void DesktopFragmentDialog::paintEvent(QPaintEvent *event)
{
	QPainter painter(this);

	painter.drawPixmap(contentsRect(), m_desktop);
	painter.setBrush(QColor(0, 0, 0, 32));
	painter.drawRect(contentsRect());

	if(!pointIsNull(m_startPoint)) {
		QRect targetRect;
		if(pointIsNull(m_stopPoint)) {
			QPoint stopPoint(qMin(QCursor::pos().x(), m_desktop.width()), qMin(QCursor::pos().y(), m_desktop.height()));

			targetRect.setTopLeft(m_startPoint);
			targetRect.setBottomRight(stopPoint);
		} else {
			targetRect.setTopLeft(m_startPoint);
			targetRect.setBottomRight(m_stopPoint);
		}

		targetRect = targetRect.normalized();

		if(m_desktop.height() - targetRect.bottom() > m_toolBar->height() + 5) {
			m_toolBar->move(targetRect.right() - m_toolBar->width() - 1, targetRect.bottom() + 5);
		} else if(targetRect.top() > m_toolBar->height() + 5) {
			m_toolBar->move(targetRect.right() - m_toolBar->width() - 1, targetRect.top() - m_toolBar->height() - 5);
		} else {
			m_toolBar->move(targetRect.right() - m_toolBar->width() - 1, targetRect.bottom() - m_toolBar->height() - 5);
		}

		m_paintBar->move(m_toolBar->x() - m_paintBar->width() - 3, m_toolBar->y());

		//selected fragment
		if(!targetRect.isNull()) {
			painter.drawPixmap(targetRect.topLeft(), m_desktop, targetRect);
			painter.drawPixmap(targetRect.topLeft(), m_painting);
		}

		//dashed frame
		painter.setPen(QPen(palette().color(QPalette::Active, QPalette::Highlight), 1, Qt::DashLine));
		painter.setBrush(Qt::transparent);
		painter.drawRect(targetRect);

		//resize handles
		if(m_mode == Moving) {
			painter.setPen(QPen(palette().color(QPalette::Active, QPalette::Highlight), 1));
			painter.setBrush(palette().color(QPalette::Active, QPalette::Highlight));

			QVector<QRect> handles;
			handles.append(QRect(targetRect.x() - 2, targetRect.y() - 2, 4, 4));                //top left
			handles.append(QRect(targetRect.x() - 2, targetRect.center().y() - 2, 4, 4));       //middle left
			handles.append(QRect(targetRect.x() - 2, targetRect.bottom() - 2, 4, 4));           //bottom left
			handles.append(QRect(targetRect.center().x() - 2, targetRect.y() - 2, 4, 4));       //top middle
			handles.append(QRect(targetRect.center().x() - 2, targetRect.bottom() - 2, 4, 4));  //bottom middle
			handles.append(QRect(targetRect.right() - 2, targetRect.y() - 2, 4, 4));            //top right
			handles.append(QRect(targetRect.right() - 2, targetRect.center().y() - 2, 4, 4));   //middle right
			handles.append(QRect(targetRect.right() - 2, targetRect.bottom() - 2, 4, 4));       //bottom right
			painter.drawRects(handles);
		}

		//hint text
		QString text = tr("Double-click to accept, right-click to select again.");
		QRect textRect = QRect(5, 5, painter.fontMetrics().width(text) + 10, painter.fontMetrics().height() + 10);
		if(textRect.contains(QCursor::pos())) {
			textRect.moveTo(5, m_desktop.height() - textRect.height() - 5);
		}

		painter.setPen(Qt::transparent);
		painter.setBrush(QColor(0, 0, 0, 128));
		painter.drawRoundedRect(textRect, 5, 5);

		painter.setPen(Qt::white);
		painter.drawText(10, textRect.top() + 15, text);

		//size text
		text = QString("%1x%2").arg(targetRect.width()).arg(targetRect.height());
		textRect = QRect(targetRect.left() + 5, targetRect.top() + 5, painter.fontMetrics().width(text) + 10, painter.fontMetrics().height() + 10);
		if(m_startPoint.x() < 20 && m_startPoint.y() < 20) {
			textRect.moveTo(targetRect.left() + 5, textRect.height() + 10);
		}

		if(textRect.contains(QCursor::pos())) {
			textRect.moveTo(targetRect.left() + 5, targetRect.bottom() - textRect.height() - 5);
		}

		painter.setPen(Qt::transparent);
		painter.setBrush(QColor(0, 0, 0, 128));
		painter.drawRoundedRect(textRect, 5, 5);

		painter.setPen(Qt::white);
		painter.drawText(textRect.left() + 5, textRect.top() + 15, text);
	} else {
		//hint text
		QString text = tr("Start draging with left mouse button to select or right-click to cancel.");
		QRect textRect = QRect(5, 5, painter.fontMetrics().width(text) + 10, painter.fontMetrics().height() + 10);
		if(textRect.contains(QCursor::pos())) {
			textRect.moveTo(5, m_desktop.height() - textRect.height() - 5);
		}

		painter.setPen(Qt::transparent);
		painter.setBrush(QColor(0, 0, 0, 128));
		painter.drawRoundedRect(textRect, 5, 5);

		painter.setPen(Qt::white);
		painter.drawText(10, textRect.top() + 15, text);

		m_toolBar->hide();
		m_paintBar->hide();
	}
}

void DesktopFragmentDialog::mousePressEvent(QMouseEvent *event)
{
	if(m_mode == Moving) {
		m_toolBar->hide();
		m_paintBar->hide();
	}

	if(pointIsNull(m_startPoint)) {
		if(event->buttons() & Qt::LeftButton) {
			m_startPoint = event->globalPos();
		} else {
			reject();
		}
	} else {
		if(event->buttons() & Qt::LeftButton) {
			if(m_mode == Moving) {
				m_dragHandle = handleAt(event->globalPos());

				if(m_dragHandle != None) {
					m_dragPoint = event->globalPos();
				}
			} else if(m_mode == Painting) {
				m_paintPoint = event->globalPos() - m_startPoint;
			}
		} else {
			m_painting = QPixmap();

			m_startPoint = QPoint(-1, -1);
			m_stopPoint = QPoint(-1, -1);

			setMovingMode();
		}
	}

	update();
}

void DesktopFragmentDialog::mouseDoubleClickEvent(QMouseEvent *event)
{
	if(!pointIsNull(m_startPoint) && !pointIsNull(m_stopPoint)) {
		accept();
	}
}

void DesktopFragmentDialog::mouseMoveEvent(QMouseEvent *event)
{
	if(!pointIsNull(m_startPoint) && !pointIsNull(m_stopPoint)) {
		if(m_mode == Moving) {
			Handle handle = handleAt(event->globalPos());

			if((handle == TopCenter) || (handle == BottomCenter)) {
				setCursor(Qt::SizeVerCursor);
			} else if((handle == MiddleLeft) || (handle == MiddleRight)) {
				setCursor(Qt::SizeHorCursor);
			} else if((handle == TopRight) || (handle == BottomLeft)) {
				setCursor(Qt::SizeBDiagCursor);
			} else if((handle == TopLeft) || (handle == BottomRight)) {
				setCursor(Qt::SizeFDiagCursor);
			} else if(handle == Inside) {
				setCursor(Qt::SizeAllCursor);
			} else {
				setCursor(Qt::ArrowCursor);
			}

			if(event->buttons() & Qt::LeftButton && !pointIsNull(m_dragPoint)) {
				int deltaX = event->globalPos().x() - m_dragPoint.x();
				int deltaY = event->globalPos().y() - m_dragPoint.y();

				QRect rect(m_startPoint, m_stopPoint);

				if(event->modifiers() & Qt::ShiftModifier) {
					rect.setHeight(rect.width());
					deltaY = deltaX;
				}

				if((m_dragHandle == Inside) || (m_dragHandle == TopLeft) || (m_dragHandle == BottomLeft) || (m_dragHandle == MiddleLeft)) {
					if((rect.left() + deltaX >= 0) && (rect.right() + deltaX <= m_desktop.width())) {
						rect.moveLeft(rect.left() + deltaX);
					}
				}

				if((m_dragHandle == Inside) || (m_dragHandle == TopLeft) || (m_dragHandle == TopRight) || (m_dragHandle == TopCenter)) {
					if((rect.top() + deltaY >= 0) && (rect.bottom() + deltaY <= m_desktop.height())) {
						rect.moveTop(rect.top() + deltaY);
					}
				}

				if((m_dragHandle == TopLeft) || (m_dragHandle == BottomLeft) || (m_dragHandle == MiddleLeft)) {
					rect.setWidth(rect.width() - deltaX);
				}

				if((m_dragHandle == TopLeft) || (m_dragHandle == TopRight) || (m_dragHandle == TopCenter)) {
					rect.setHeight(rect.height() - deltaY);
				}

				if((m_dragHandle == TopRight) || (m_dragHandle == BottomRight) || (m_dragHandle == MiddleRight)) {
					rect.setWidth(rect.width() + deltaX);
				}

				if((m_dragHandle == BottomRight) || (m_dragHandle == BottomLeft) || (m_dragHandle == BottomCenter)) {
					rect.setHeight(rect.height() + deltaY);
				}

				if(rect.width()  < 1) { rect.setWidth(1);  }
				if(rect.height() < 1) { rect.setHeight(1); }
				if(rect.left()   < 0) { rect.setLeft(0);   }
				if(rect.top()    < 0) { rect.setTop(0);    }

				if(rect.right()  > m_desktop.width())  { rect.setRight(m_desktop.width());   }
				if(rect.bottom() > m_desktop.height()) { rect.setBottom(m_desktop.height()); }

				rect = rect.normalized();

				if(rect.size() != m_painting.size()) {
					if(!m_painting.isNull()) {
						m_painting = QPixmap(rect.width(), rect.height());
						m_painting.fill(Qt::transparent);
					}
				}

				m_startPoint = rect.topLeft();
				m_stopPoint = rect.bottomRight();

				m_dragPoint = event->globalPos();
			}
		} else if(m_mode == Painting) {
			setCursor(Qt::ArrowCursor);

			if(event->buttons() & Qt::LeftButton) {
				QColor color = qvariant_cast<QColor>(m_colorAction->data());

				QPainter p(&m_painting);

				//eraser
				if(color == Qt::transparent) {
					p.setCompositionMode(QPainter::CompositionMode_Clear);
				}

				p.setPen(QPen(color, m_penSlider->value(), Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
				p.drawLine(m_paintPoint, event->globalPos() - m_startPoint);

				p.end();

				m_paintPoint = event->globalPos() - m_startPoint;
			}
		}
	} else {
		setCursor(Qt::ArrowCursor);
	}

	update();
}

void DesktopFragmentDialog::mouseReleaseEvent(QMouseEvent *event)
{
	if(!pointIsNull(m_startPoint) && pointIsNull(m_stopPoint)) {
		if(m_startPoint == event->globalPos()) {
			m_startPoint = QPoint(-1, -1);
			return;
		}

		QRect rect(m_startPoint, event->globalPos());
		rect = rect.normalized();

		m_startPoint = rect.topLeft();
		m_stopPoint = QPoint(qMin(rect.bottomRight().x(), m_desktop.width()), qMin(rect.bottomRight().y(), m_desktop.height()));
	}

	m_dragHandle = None;
	m_dragPoint = QPoint(-1, -1);

	update();

	m_toolBar->show();
}

void DesktopFragmentDialog::keyPressEvent(QKeyEvent *event)
{
	if(event->key() == Qt::Key_Return) {
		if(!pointIsNull(m_startPoint) && !pointIsNull(m_stopPoint)) {
			accept();
		}
	}

	QDialog::keyPressEvent(event);
}

void DesktopFragmentDialog::setMovingMode()
{
	setMode(Moving);
}

void DesktopFragmentDialog::setPaintingMode()
{
	setMode(Painting);
}

void DesktopFragmentDialog::setMode(DesktopFragmentDialog::Mode mode)
{
	m_toolBar->actions().at(0)->setChecked(mode == Moving);
	m_toolBar->actions().at(1)->setChecked(mode == Painting);

	m_mode = mode;

	if(mode == Painting) {
		if(m_painting.isNull()) {
			m_painting = QPixmap(QRect(m_startPoint, m_stopPoint).size());
			m_painting.fill(Qt::transparent);
		}
	}

	m_paintBar->setVisible(mode == Painting);

	update();
}

void DesktopFragmentDialog::changePenColor()
{
	QColor color = qvariant_cast<QColor>(m_colorAction->data());
	color = QColorDialog::getColor(color, this);

	QPixmap icon(16, 16);
	icon.fill(color);

	m_colorAction->setIcon(icon);
	m_colorAction->setData(color);
}

void DesktopFragmentDialog::setEraserColor()
{
	QPixmap icon(16, 16);
	icon.fill(Qt::white);

	QPainter p(&icon);
	p.fillRect(0, 0, 8, 8, Qt::gray);
	p.fillRect(8, 8, 16, 16, Qt::gray);
	p.end();

	m_colorAction->setIcon(icon);
	m_colorAction->setData(QColor(Qt::transparent));
}

void DesktopFragmentDialog::clearPainting()
{
	if(!m_painting.isNull()) {
		m_painting.fill(Qt::transparent);
		update();
	}
}

DesktopFragmentDialog::Handle DesktopFragmentDialog::handleAt(const QPoint &p)
{
	QRect rect(m_startPoint, m_stopPoint);

	const QRect topLeft(rect.x() - 2, rect.y() - 2, 4, 4);
	const QRect topCenter(rect.center().x() - 2, rect.y() - 2, 4, 4);
	const QRect topRight(rect.right() - 2, rect.y() - 2, 4, 4);

	const QRect middleLeft(rect.x() - 2, rect.center().y() - 2, 4, 4);
	const QRect middleRight(rect.right() - 2, rect.center().y() - 2, 4, 4);

	const QRect bottomLeft(rect.x() - 2, rect.bottom() - 2, 4, 4);
	const QRect bottomCenter(rect.center().x() - 2, rect.bottom() - 2, 4, 4);
	const QRect bottomRight(rect.right() - 2, rect.bottom() - 2, 4, 4);

	if(topLeft.contains(p)) {
		return TopLeft;
	} else if(topCenter.contains(p)) {
		return TopCenter;
	} else if(topRight.contains(p)) {
		return TopRight;
	} else if(middleLeft.contains(p)) {
		return MiddleLeft;
	} else if(middleRight.contains(p)) {
		return MiddleRight;
	} else if(bottomLeft.contains(p)) {
		return BottomLeft;
	} else if(bottomCenter.contains(p)) {
		return BottomCenter;
	} else if(bottomRight.contains(p)) {
		return BottomRight;
	} else if(rect.contains(p, true)) {
		return Inside;
	}

	return None;
}

void DesktopFragmentDialog::showEvent(QShowEvent *event)
{
	m_desktop = QPixmap::grabWindow(QApplication::desktop()->winId());
	QDialog::showEvent(event);
}
