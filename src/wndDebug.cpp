#include "wndDebug.h"
#include "ui_wndDebug.h"

#include "KittyCore.h"
#include "KittyActionMngr.h"

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

  connect(m_ui->execButton, SIGNAL(clicked()), this, SLOT(execCommand()));
  connect(m_ui->commandEdit, SIGNAL(returnPressed()), m_ui->execButton, SLOT(click()));
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

void wndDebug::showEvent(QShowEvent *event)
{
  QWidget::showEvent(event);

  m_ui->tabWidget->setCurrentIndex(0);
  m_ui->refreshActionsButton->click();
}

void wndDebug::execCommand()
{
  QStringList commands = m_ui->commandEdit->text().split(" ");
  QString msg = QString("<div>%1</div>").arg(tr("Unknown command, type <i>help</i> for list of commands."));

  if(commands.count() > 0) {
    if(commands.at(0) == "help") {
      msg = QString("<div>%1</div>").arg(tr("Commands:<br><i>help</i> - prints this text<br><i>quit</i> - quit KittyIM"));
    } else if(commands.at(0) == "quit") {
      qApp->quit();
    }

    m_wvLog->setHtml(m_wvLog->page()->mainFrame()->toHtml() + QString("<div>&gt; %1</div>").arg(m_ui->commandEdit->text()));
    m_wvLog->setHtml(m_wvLog->page()->mainFrame()->toHtml() + msg);
  }

  m_history.append(m_ui->commandEdit->text());
  m_ui->commandEdit->clear();
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

void wndDebug::on_refreshActionsButton_clicked()
{
  m_ui->actionsWidget->clear();

  QStringList keys = KittyCore::inst()->actionManager()->keys();
  qSort(keys);
  foreach(QString key, keys) {
    QString prefix;
    QStringList parts = key.split(".");
    if(parts.count() > 1) {
      prefix = parts.at(0);
    }

    QTreeWidgetItem *item = new QTreeWidgetItem();

    item->setText(0, key);
    item->setIcon(0, KittyCore::inst()->action(key)->icon());

    if(prefix.isEmpty()) {
      m_ui->actionsWidget->addTopLevelItem(item);
    } else {
      QList<QTreeWidgetItem*> items = m_ui->actionsWidget->findItems(prefix, Qt::MatchExactly);
      if(items.count() > 0) {
        items.at(0)->addChild(item);
      } else {
        QTreeWidgetItem *parent = new QTreeWidgetItem();
        parent->setText(0, prefix);
        parent->addChild(item);
        m_ui->actionsWidget->addTopLevelItem(parent);
      }
    }
  }
}
