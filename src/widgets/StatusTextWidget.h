#ifndef STATUSTEXTWIDGET_H
#define STATUSTEXTWIDGET_H

#include <QtGui/QToolButton>
#include <QtGui/QLineEdit>

namespace Kitty
{
  class StatusLineEdit: public QLineEdit
  {
    Q_OBJECT

    public:
      explicit StatusLineEdit(QWidget *parent = 0);

      QSize sizeHint() const;

    private slots:
      void updateSize();

    protected:
      void paintEvent(QPaintEvent *event);
      void keyPressEvent(QKeyEvent *event);
  };

  class StatusArrowButton: public QToolButton
  {
    Q_OBJECT

    public:
      explicit StatusArrowButton(QWidget *parent = 0): QToolButton(parent) { }

      QSize sizeHint() const { return QSize(20, 20); }

    protected:
      void paintEvent(QPaintEvent *event);
  };

  class StatusTextWidget: public QWidget
  {
    Q_OBJECT

    public:
      explicit StatusTextWidget(QWidget *parent = 0);

      QString text() const;
      void setText(const QString &text);

    signals:
      void returnPressed(QString text);

    private slots:
      void changeText();

    private:
      StatusLineEdit *m_lineEdit;

  };
}

#endif
