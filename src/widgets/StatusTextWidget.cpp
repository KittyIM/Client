#include "StatusTextWidget.h"

#include <QtGui/QStyleOptionComboBox>
#include <QtGui/QHBoxLayout>
#include <QtGui/QStylePainter>
#include <QtGui/QKeyEvent>
#include <QtGui/QToolTip>
#include <QtGui/QMenu>

using namespace Kitty;

Kitty::StatusLineEdit::StatusLineEdit(QWidget *parent): QLineEdit(parent)
{
	connect(this, SIGNAL(textChanged(QString)), this, SLOT(updateSize()));
}

QSize Kitty::StatusLineEdit::sizeHint() const
{
	return QSize(4 + fontMetrics().width(text()) + 2, 20);
}

void Kitty::StatusLineEdit::updateSize()
{
	updateGeometry();
}

void Kitty::StatusLineEdit::paintEvent(QPaintEvent *event)
{
	if(hasFocus()) {
		QLineEdit::paintEvent(event);
	} else {
		QStylePainter painter(this);

		if(underMouse()) {
			QStyleOptionButton opt;
			opt.initFrom(this);

			opt.state &= ~QStyle::State_HasFocus;

			painter.drawControl(QStyle::CE_PushButton, opt);
		}

		painter.setPen(palette().color(QPalette::Text));

		QRect textRect = rect();
		textRect.moveLeft(4);
		textRect.moveTop(1);

		if(text().isEmpty()) {
			painter.setPen(palette().color(QPalette::Dark));

			QFont font = painter.font();
			font.setItalic(true);
			painter.setFont(font);

			painter.drawText(textRect, Qt::AlignVCenter | Qt::TextSingleLine, tr("Description..."));
		} else {
			painter.drawText(textRect, Qt::AlignVCenter | Qt::TextSingleLine, painter.fontMetrics().elidedText(text(), Qt::ElideRight, width() - 4));
		}
	}
}

void Kitty::StatusLineEdit::keyPressEvent(QKeyEvent *event)
{
	QLineEdit::keyPressEvent(event);

	if(!event->text().isEmpty()) {
		QToolTip::showText(mapToGlobal(QPoint()), QString::number(text().length()));
	}
}


void Kitty::StatusArrowButton::paintEvent(QPaintEvent *event)
{
	QStylePainter painter(this);

	if(underMouse()) {
		QStyleOptionButton opt;
		opt.initFrom(this);

		if(isDown()) {
			opt.state |= QStyle::State_Sunken;
		}

		painter.drawControl(QStyle::CE_PushButton, opt);
	}

	QStyleOptionComboBox opt;
	opt.initFrom(this);
	opt.rect = QRect(width() - 15, height() / 2 - 6, 11, 13);
	painter.drawPrimitive(QStyle::PE_IndicatorArrowDown, opt);
}


Kitty::StatusTextWidget::StatusTextWidget(QWidget *parent): QWidget(parent)
{
	m_lineEdit = new StatusLineEdit(this);
	connect(m_lineEdit, SIGNAL(returnPressed()), this, SLOT(changeText()));

	StatusArrowButton *arrow = new StatusArrowButton(this);
	connect(arrow, SIGNAL(clicked()), this, SLOT(showHistory()));

	QHBoxLayout *lay = new QHBoxLayout(this);
	lay->setContentsMargins(QMargins());
	lay->setSpacing(0);
	lay->addWidget(m_lineEdit);
	lay->addWidget(arrow);
	setLayout(lay);
}

QString Kitty::StatusTextWidget::text() const
{
	return m_lineEdit->text();
}

void Kitty::StatusTextWidget::setText(const QString &text)
{
	m_lineEdit->setText(text);
}

void Kitty::StatusTextWidget::changeText()
{
	if(!text().isEmpty()) {
		m_history.removeAll(text());
		m_history.prepend(text());
	}

	emit returnPressed(text());
}

void Kitty::StatusTextWidget::showHistory()
{
	QMenu menu;
	foreach(const QString &descr, m_history) {
		QAction *action = menu.addAction((descr.count() > 65)?(descr.left(65) + "..."):descr, this, SLOT(applyHistory()));
		action->setToolTip(descr);
	}
	menu.exec(mapToGlobal(QPoint(0, height())));
}

void Kitty::StatusTextWidget::applyHistory()
{
}
