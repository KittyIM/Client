#include "ChatColorPicker.h"

#include <QtGui/QColorDialog>
#include <QtGui/QGridLayout>
#include <QtGui/QPushButton>

using namespace Kitty;

Kitty::ChatColorFrame::ChatColorFrame(const QString &color, QWidget *parent): QFrame(parent)
{
  setStyleSheet(QString("background-color: %1;").arg(color));
  setMinimumSize(QSize(16, 16));
  setMaximumSize(QSize(16, 16));
  setToolTip(QColor(color).name());
  setCursor(Qt::PointingHandCursor);
}

void Kitty::ChatColorFrame::enterEvent(QEvent *event)
{
  setFrameShape(QFrame::Box);
}

void Kitty::ChatColorFrame::leaveEvent(QEvent *event)
{
  setFrameShape(QFrame::NoFrame);
}

void Kitty::ChatColorFrame::mousePressEvent(QMouseEvent *event)
{
  emit clicked(palette().window().color());
}

Kitty::ChatColorPicker::ChatColorPicker(QWidget *parent): QWidget(parent, Qt::Popup)
{
  QGridLayout *gridLayout = new QGridLayout(this);
  gridLayout->setSpacing(2);
  gridLayout->setContentsMargins(2, 2, 2, 2);

  QStringList colors;
  colors << "black" << "gray" << "maroon" << "red" << "chocolate";
  colors << "white" << "silver" << "tan" << "lightpink" << "orange";
  colors << "yellow" << "forestgreen" << "deepskyblue" << "royalblue" << "purple";
  colors << "khaki" << "yellowgreen" << "lightblue" << "slategray" << "thistle";

  int width = 5;

  int i = 0;
  foreach(QString color, colors) {
    ChatColorFrame *frame = new ChatColorFrame(color, this);

    connect(frame, SIGNAL(clicked(QColor)), this, SIGNAL(colorSelected(QColor)));
    connect(frame, SIGNAL(clicked(QColor)), this, SLOT(hide()));

    gridLayout->addWidget(frame, i / width, i % width);

    i++;
  }

  m_moreButton = new QPushButton(this);
  m_moreButton->setFlat(true);
  connect(m_moreButton, SIGNAL(clicked()), this, SLOT(customColor()));
  gridLayout->addWidget(m_moreButton, i / width + 1, 0, 1, 5);

  retranslate();
}

void Kitty::ChatColorPicker::showAt(const QPoint &pos)
{
  move(pos);
  resize(sizeHint());
  show();
}

void Kitty::ChatColorPicker::retranslate()
{
  m_moreButton->setText(tr("More..."));
}

void Kitty::ChatColorPicker::customColor()
{
  QColor color = QColorDialog::getColor(Qt::black, this);
  emit colorSelected(color);
}
