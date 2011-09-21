#ifndef HISTORYWINDOW_H
#define HISTORYWINDOW_H

#include <QtGui/QWidget>
#include <QSortFilterProxyModel>

namespace Ui
{
  class HistoryWindow;
}

namespace Kitty
{
  class ContactProxy: public QSortFilterProxyModel
  {
    Q_OBJECT

    public:
      explicit ContactProxy(QObject *parent = 0);

    protected:
      bool filterAcceptsRow(int row, const QModelIndex &parent) const;
  };

  class HistoryWindow: public QWidget
  {
    Q_OBJECT

    public:
      explicit HistoryWindow(QWidget *parent = 0);
      ~HistoryWindow();

    public slots:
      void applySettings();
      void updateIcons();

    protected:
      void showEvent(QShowEvent *event);

    private slots:
      void on_contactSearchEdit_textChanged(const QString &text);

    private:
      Ui::HistoryWindow *m_ui;
      class ContactProxy *m_proxy;
  };
}

#endif // HISTORYWINDOW_H
