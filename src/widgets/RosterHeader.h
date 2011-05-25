#ifndef ROSTERHEADER_H
#define ROSTERHEADER_H

#include <QtGui/QWidget>

namespace Ui
{
  class RosterHeader;
}

namespace Kitty
{
  class RosterHeader: public QWidget
  {
    Q_OBJECT

    public:
      explicit RosterHeader(QWidget *parent = 0);
      ~RosterHeader();

    signals:
      void descriptionChanged(QString description);

    public slots:
      void applySettings();

    protected:
      void paintEvent(QPaintEvent *event);

    private:
      Ui::RosterHeader *m_ui;
  };
}

#endif