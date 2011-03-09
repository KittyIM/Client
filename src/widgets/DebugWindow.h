#ifndef DEBUGWINDOW_H
#define DEBUGWINDOW_H

#include <QtGui/QWidget>

#include <windows.h>

class QStringList;
class QWebView;

namespace Ui
{
  class DebugWindow;
}

namespace Kitty
{
  class DebugWindow: public QWidget
  {
    Q_OBJECT

    public:
      ~DebugWindow();

      static DebugWindow *inst();
      static void destroy();

      static void addMessage(QtMsgType type, const char *msg);

    protected:
      void showEvent(QShowEvent *event);

    private slots:
      void execCommand();
      void on_refreshActionsButton_clicked();
      void on_refreshIconsButton_clicked();

    private:
      explicit DebugWindow();
      DebugWindow(const DebugWindow &);
      void operator=(const DebugWindow &);

    private:
      Ui::DebugWindow *m_ui;
      QStringList m_history;
      static DebugWindow *m_inst;
      static QWebView *m_wvLog;
  };
}

#endif // DEBUGWINDOW_H
