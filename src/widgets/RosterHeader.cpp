#include "RosterHeader.h"
#include "ui_RosterHeader.h"

#include "AccountManager.h"
#include "Profile.h"
#include "Core.h"

#include <QtCore/QDebug>
#include <QtGui/QStylePainter>

namespace Kitty
{

RosterHeader::RosterHeader(QWidget *parent): QWidget(parent), m_ui(new Ui::RosterHeader)
{
	m_ui->setupUi(this);

	connect(m_ui->statusTextEdit, SIGNAL(returnPressed(QString)), this, SIGNAL(descriptionChanged(QString)));
	connect(m_ui->statusChangeButton, SIGNAL(statusChanged(KittySDK::IProtocol::Status)), this, SIGNAL(statusChanged(KittySDK::IProtocol::Status)));
	connect(AccountManager::inst(), SIGNAL(accountStatusChanged(KittySDK::IAccount*,KittySDK::IProtocol::Status,QString)), this, SLOT(updateWidgets()));
}

RosterHeader::~RosterHeader()
{
	delete m_ui;
}

void RosterHeader::applySettings()
{
	Core *core = Core::inst();

	m_ui->profileLabel->setText(core->profileName());
	m_ui->avatarLabel->setPixmap(QPixmap(core->profilesDir() + core->profileName() + "/avatar.png"));
}

void RosterHeader::updateWidgets()
{
	KittySDK::IProtocol::Status status = KittySDK::IProtocol::Offline;
	foreach(KittySDK::IAccount *acc, AccountManager::inst()->accounts()) {
		status = qMin(status, acc->status());
	}

	m_ui->statusChangeButton->setStatus(status);
}

void RosterHeader::retranslate()
{
	m_ui->statusChangeButton->retranslate();
}

void RosterHeader::paintEvent(QPaintEvent *event)
{
	QStyleOption opt;
	opt.init(this);

	QStylePainter p(this);
	p.drawPrimitive(QStyle::PE_Widget, opt);
}

}
