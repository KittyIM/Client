#include "wndDebug.h"
#include "ui_wndDebug.h"

#include <QtCore/QDebug>
#include <QtCore/QMutex>
#include <QtCore/QDateTime>
#include <QtGui/QMessageBox>
#include <QtWebKit/QWebView>
#include <QtWebKit/QWebFrame>

wndDebug *wndDebug::m_inst = 0;
QWebView *wndDebug::m_wvLog = 0;

wndDebug::wndDebug(): QWidget(0), m_ui(new Ui::wndDebug)
{
  m_ui->setupUi(this);

  m_wvLog = new QWebView(this);
  m_wvLog->setHtml("<html><head><style type=\"text/css\">body { font-family: Tahoma; font-size: 11px; }</style></head><body></body></html>");

  m_ui->verticalLayout->insertWidget(0, m_wvLog);
}

wndDebug::~wndDebug()
{
  delete m_ui;
}

void wndDebug::addMessage(QtMsgType type, const char *msg)
{
  wndDebug::inst();

  QString color = "#ffffff";
  switch(type) {
    case QtDebugMsg:
      color = "#ccffcc";
    break;

    case QtWarningMsg:
      color = "#ffffcc";
    break;

    case QtCriticalMsg:
      color = "#ffcc99";
    break;

    case QtFatalMsg:
      color = "#ffcccc";
    break;
  }

  m_wvLog->setHtml(m_wvLog->page()->mainFrame()->toHtml() + QString("<div style='background-color: %1;'>[%2] %3</div>").arg(color).arg(QDateTime::currentDateTime().toString("hh:mm:ss")).arg(msg));
}

wndDebug *wndDebug::inst()
{
  static QMutex mutex;

  if(!m_inst) {
    mutex.lock();
    m_inst = new wndDebug();
    mutex.unlock();
  }

  return m_inst;
}

void wndDebug::destroy()
{
  static QMutex mutex;

  if(m_inst) {
    mutex.lock();
    delete m_inst;
    m_inst = 0;
    mutex.unlock();
  }
}
