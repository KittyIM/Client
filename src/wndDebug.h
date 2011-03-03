#ifndef WNDDEBUG_H
#define WNDDEBUG_H

#include <QtGui/QWidget>

class QStringList;
class QWebView;

namespace Ui
{
  class wndDebug;
}

class wndDebug: public QWidget
{
  Q_OBJECT

  public:
    ~wndDebug();

    static wndDebug *inst();
    static void destroy();

    static void addMessage(QtMsgType type, const char *msg);

  protected:
    void showEvent(QShowEvent *event);

  private slots:
    void execCommand();

    void on_refreshActionsButton_clicked();

private:
    explicit wndDebug();
    wndDebug(const wndDebug &);
    void operator=(const wndDebug &);

  private:
    Ui::wndDebug *m_ui;
    QStringList m_history;
    static wndDebug *m_inst;
    static QWebView *m_wvLog;
};

#endif // WNDDEBUG_H
