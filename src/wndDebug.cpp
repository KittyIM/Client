#include "wndDebug.h"
#include "ui_wndDebug.h"

#include <QtCore/QDateTime>
#include <QtCore/QMutex>
#include <QtGui/QMessageBox>
#include <QtWebKit/QWebView>
#include <QtWebKit/QWebFrame>

wndDebug *wndDebug::m_inst = 0;
QWebView *wndDebug::m_wvLog = 0;

wndDebug::wndDebug(): QWidget(0), m_ui(new Ui::wndDebug)
{
  m_ui->setupUi(this);

  m_wvLog = new QWebView(this);
  m_ui->verticalLayout->insertWidget(0, m_wvLog);
}

wndDebug::~wndDebug()
{
  delete m_ui;
}

void wndDebug::addMessage(QtMsgType type, const char *msg)
{
  wndDebug::inst();

  QString color = "white";
  switch(type) {
    case QtDebugMsg:
      color = "#CCFFCC";
    break;

    case QtWarningMsg:
      color = "#FFFFCC";
    break;

    case QtCriticalMsg:
      color = "#FFCC99";
    break;

    case QtFatalMsg:
      color = "#FFCCCC";
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
