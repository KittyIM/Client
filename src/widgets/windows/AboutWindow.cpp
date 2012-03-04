#include "AboutWindow.h"
#include "ui_AboutWindow.h"

#include "3rdparty/qtwin/qtwin.h"
#include "PluginManager.h"
#include <IPlugin.h>
#include "constants.h"

#include <QtCore/QDebug>

#define qDebug() qDebug() << "[AboutWindow]"
#define qWarning() qWarning() << "[AboutWindow]"

namespace Kitty
{

AboutWindow::AboutWindow(QWidget *parent): QDialog(parent), m_ui(new Ui::AboutWindow)
{
	m_ui->setupUi(this);

	setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
	setAttribute(Qt::WA_DeleteOnClose);

	if(QtWin::isCompositionEnabled()) {
		QtWin::extendFrameIntoClientArea(this);
		setContentsMargins(0, 0, 0, 0);
	}

	//qDebug() << "Creating";
}

AboutWindow::~AboutWindow()
{
	//qDebug() << "Destroying";

	delete m_ui;
}

void AboutWindow::showEvent(QShowEvent *event)
{
	QDialog::showEvent(event);
	updateAboutText();
}

void AboutWindow::changeEvent(QEvent *event)
{
	m_ui->retranslateUi(this);
	updateAboutText();
}

void AboutWindow::updateAboutText()
{
	QMap<QString, QString> programmers;

	programmers.insert("Artur \"arturo182\" Pacholec", "arturo182@tlen.pl");

	QString text = QString("<b>KittyIM v%1 %2 bit</b><br>").arg(Constants::VERSION).arg(QSysInfo::WordSize);
	text.append(tr("Built on %1 at %2.").arg(__DATE__).arg(__TIME__));
	text.append(QString("<br><br><i>%1</i><br>").arg(tr("Programming") + ":"));

	QMapIterator<QString, QString> i(programmers);
	while(i.hasNext()) {
		i.next();
		text.append(QString("%1 &lt;<a href=\"mailto:%2\">%2</a>&gt;<br>").arg(i.key()).arg(i.value()));
	}

	text.append(QString("<br><b>%1</b><br>").arg(tr("Plugins")));

	foreach(Plugin *plug, PluginManager::inst()->plugins()) {
		if(plug->hasError()) {
			continue;
		}

		if(KittySDK::IPlugin *iplugin = plug->iplugin()) {
			if(KittySDK::IPluginInfo *info = iplugin->info()) {
				text.append(QString("<br><i>%1 %2</i><br>%3 &lt;<a href=\"mailto:%4\">%4</a>&gt;<br><a href=\"%5\">%5</a><br>").arg(info->name()).arg(info->version()).arg(info->author()).arg(info->email()).arg(info->url()));
			}
		}
	}

	m_ui->aboutText->setText(text);
}

}
