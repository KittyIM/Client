#include "DebugWindow.h"
#include "ui_DebugWindow.h"

#include "ActionManager.h"
#include "IconManager.h"
#include "MainWindow.h"
#include "Profile.h"
#include "Core.h"
#include "App.h"

#include <QtCore/QDateTime>
#include <QtCore/QDebug>
#include <QtCore/QMutex>
#include <QtGui/QClipboard>
#include <QtGui/QKeyEvent>
#include <QtWebKit/QWebFrame>
#include <QtWebKit/QWebView>

#define qDebug() qDebug() << "[DebugWindow]"
#define qWarning() qWarning() << "[DebugWindow]"

QWebView *Kitty::DebugWindow::m_wvLog = 0;

namespace Kitty
{

DebugWindow::DebugWindow(Core *core, QWidget *parent):
	QWidget(parent),
	m_ui(new Ui::DebugWindow),
	m_core(core)
{
	m_ui->setupUi(this);

	setWindowFlags(windowFlags() & ~Qt::WindowMinMaxButtonsHint);

	m_wvLog = new QWebView(this);
	m_wvLog->installEventFilter(this);
	m_wvLog->setContextMenuPolicy(Qt::NoContextMenu);
	m_wvLog->setHtml("<html><head><meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\"><style type=\"text/css\">body { font-family: Tahoma; font-size: 11px; margin: 2px; }</style></head><body></body></html>");

	m_ui->consoleGridLayout->addWidget(m_wvLog);

	connect(m_ui->execButton, SIGNAL(clicked()), this, SLOT(execCommand()));
	connect(m_ui->commandEdit, SIGNAL(returnPressed()), m_ui->execButton, SLOT(click()));
}


DebugWindow::~DebugWindow()
{
	qInstallMsgHandler(0);

	delete m_wvLog;

	delete m_ui;
}

void DebugWindow::addMessage(QtMsgType type, const char *msg)
{
	Core::inst()->debugWindow();

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

	m_wvLog->setHtml(m_wvLog->page()->mainFrame()->toHtml() + QString("<span style='background-color: %1;'>&nbsp;&nbsp;&nbsp;</span> <b>##%2</b> %3<br>").arg(color).arg(QDateTime::currentDateTime().toString("hh:mm:ss")).arg(QString::fromLocal8Bit(msg).replace(" ", "&nbsp;")));
	m_wvLog->page()->mainFrame()->setScrollBarValue(Qt::Vertical, m_wvLog->page()->mainFrame()->scrollBarMaximum(Qt::Vertical));
}

void DebugWindow::showEvent(QShowEvent *event)
{
	m_ui->tabWidget->setCurrentIndex(0);
	m_ui->commandEdit->setFocus();
}

void DebugWindow::changeEvent(QEvent *event)
{
	if(event->type() == QEvent::LanguageChange) {
		m_ui->retranslateUi(this);
	}

	QWidget::changeEvent(event);
}

bool DebugWindow::eventFilter(QObject *obj, QEvent *event)
{
	if(event->type() == QEvent::KeyPress) {
		if(QKeyEvent *keyEvent = static_cast<QKeyEvent*>(event)) {
			if((keyEvent->key() == Qt::Key_C) && (keyEvent->modifiers() & Qt::ControlModifier)) {
				qApp->clipboard()->setText(m_wvLog->selectedText());
			}
		}
	}

	return QObject::eventFilter(obj, event);
}

void DebugWindow::execCommand()
{
	QStringList commands = m_ui->commandEdit->text().split(" ");
	QString msg = QString("<div>%1</div>").arg(tr("Unknown command, type <i>help</i> for list of commands."));

	if(commands.count() > 0) {
		if(commands.at(0) == "help") {
			msg = QString("<div>%1</div>").arg(tr("Commands:<br><i>help</i> - prints this text<br><i>uptime</i> - show uptime<br><i>quit</i> - quit KittyIM"));
		} else if(commands.at(0) == "quit") {
			qApp->quit();
		} else if(commands.at(0) == "clear") {
			m_wvLog->setHtml("<html><head><meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\"><style type=\"text/css\">body { font-family: Tahoma; font-size: 11px; margin: 2px; }</style></head><body></body></html>");
			msg = "";
		} else if(commands.at(0) == "uptime") {
			int secs = static_cast<App*>(qApp)->startDate().secsTo(QDateTime::currentDateTime());

			msg = QString("Uptime: %1h %2m %3s").arg(secs / 3600).arg((secs / 60) % 60).arg(secs % 60);
		}

		m_wvLog->setHtml(m_wvLog->page()->mainFrame()->toHtml() + QString("<div>&gt; %1</div>").arg(m_ui->commandEdit->text()));
		m_wvLog->setHtml(m_wvLog->page()->mainFrame()->toHtml() + msg);
		m_wvLog->page()->mainFrame()->setScrollBarValue(Qt::Vertical, m_wvLog->page()->mainFrame()->scrollBarMaximum(Qt::Vertical));

		m_history.append(m_ui->commandEdit->text());
	}
	m_ui->commandEdit->clear();
}

void DebugWindow::on_refreshActionsButton_clicked()
{
	m_ui->actionsWidget->clear();

	QStringList keys = m_core->actionManager()->keys();
	qSort(keys);
	foreach(QString key, keys) {
		QString prefix;
		QStringList parts = key.split(".");
		if(parts.count() > 1) {
			prefix = parts.at(0);
		}

		QTreeWidgetItem *item = new QTreeWidgetItem();

		item->setText(0, key);
		item->setIcon(0, m_core->action(key)->icon());

		if(prefix.isEmpty()) {
			m_ui->actionsWidget->addTopLevelItem(item);
		} else {
			QList<QTreeWidgetItem*> items = m_ui->actionsWidget->findItems(prefix, Qt::MatchExactly);
			if(items.count() > 0) {
				items.at(0)->addChild(item);
			} else {
				QTreeWidgetItem *parent = new QTreeWidgetItem(m_ui->actionsWidget);
				parent->setText(0, prefix);
				parent->addChild(item);
			}
		}
	}
}

void DebugWindow::on_refreshIconsButton_clicked()
{
	m_ui->iconsWidget->clear();

	QStringList keys = m_core->iconManager()->keys();
	qSort(keys);
	foreach(QString key, keys) {
		QString prefix;
		QStringList parts = key.split(".");
		if(parts.count() > 1) {
			prefix = parts.at(0);
		}

		QTreeWidgetItem *item = new QTreeWidgetItem();

		item->setText(0, key);
		item->setIcon(0, m_core->icon(key));

		if(prefix.isEmpty()) {
			m_ui->iconsWidget->addTopLevelItem(item);
		} else {
			QList<QTreeWidgetItem*> items = m_ui->iconsWidget->findItems(prefix, Qt::MatchExactly);
			if(items.count() > 0) {
				items.at(0)->addChild(item);
			} else {
				QTreeWidgetItem *parent = new QTreeWidgetItem(m_ui->iconsWidget);
				parent->setText(0, prefix);
				parent->addChild(item);
			}
		}
	}
}

void DebugWindow::on_iconsWidget_currentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous)
{
	if(current) {
		m_ui->iconPreviewLabel->setPixmap(m_core->icon(current->text(0)));
	} else {
		m_ui->iconPreviewLabel->setPixmap(QPixmap());
	}
}

}
