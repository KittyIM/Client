#include "StatusChangeButton.h"

#include "IconManager.h"
#include "Core.h"

#include <SDKConstants.h>

#include <QtCore/QDebug>
#include <QtGui/QStyleOptionToolButton>
#include <QtGui/QStylePainter>
#include <QtGui/QMenu>

namespace Kitty
{

StatusChangeButton::StatusChangeButton(QWidget *parent):
	QToolButton(parent)
{
	Core *core = Core::inst();
	QMenu *menu = new QMenu(this);

	connect(IconManager::inst(), SIGNAL(iconsUpdated()), this, SLOT(updateIcons()));

	m_onlineAction = menu->addAction(core->icon(KittySDK::Icons::I_STATUS_ONLINE), tr("Online"), this, SLOT(setStatusOnline()));
	m_awayAction = menu->addAction(core->icon(KittySDK::Icons::I_STATUS_AWAY), tr("Away"), this, SLOT(setStatusAway()));
	m_FFCAction = menu->addAction(core->icon(KittySDK::Icons::I_STATUS_FFC), tr("Free for chat"), this, SLOT(setStatusFFC()));
	m_DNDAction = menu->addAction(core->icon(KittySDK::Icons::I_STATUS_DND), tr("Do not disturb"), this, SLOT(setStatusDND()));
	m_InvisibleAction = menu->addAction(core->icon(KittySDK::Icons::I_STATUS_INVIS), tr("Invisible"), this, SLOT(setStatusInvisible()));
	m_OfflineAction = menu->addAction(core->icon(KittySDK::Icons::I_STATUS_OFFLINE), tr("Offline"), this, SLOT(setStatusOffline()));

	setMenu(menu);
	setPopupMode(QToolButton::InstantPopup);
	setIcon(core->icon(KittySDK::Icons::I_STATUS_OFFLINE));

	m_status = KittySDK::IProtocol::Offline;
}

QSize StatusChangeButton::sizeHint() const
{
	return QSize(2 + iconSize().width() + 2 + fontMetrics().width(text()) + 24, 20);
}

void StatusChangeButton::retranslate()
{
	m_onlineAction->setText(tr("Online"));
	m_awayAction->setText(tr("Away"));
	m_FFCAction->setText(tr("Free for chat"));
	m_DNDAction->setText(tr("Do not disturb"));
	m_InvisibleAction->setText(tr("Invisible"));
	m_OfflineAction->setText(tr("Offline"));

	setStatus(m_status);
}

void StatusChangeButton::setStatus(KittySDK::IProtocol::Status status)
{
	switch(status) {
		case KittySDK::IProtocol::Online:
			setStatusOnline(false);
		break;

		case KittySDK::IProtocol::Away:
			setStatusAway(false);
		break;

		case KittySDK::IProtocol::FFC:
			setStatusFFC(false);
		break;

		case KittySDK::IProtocol::DND:
			setStatusDND(false);
		break;

		case KittySDK::IProtocol::Invisible:
			setStatusInvisible(false);
		break;

		case KittySDK::IProtocol::Offline:
			setStatusOffline(false);
		break;

		default:
			qWarning() << "Unknown status" << status;
		break;
	}
}

void StatusChangeButton::paintEvent(QPaintEvent *event)
{
	QStylePainter painter(this);

	if(underMouse()) {
		QStyleOptionButton opt;
		opt.initFrom(this);

		if(isDown()) {
			opt.state |= QStyle::State_Sunken;
		}

		painter.drawControl(QStyle::CE_PushButton, opt);
	}

	painter.setPen(palette().color(QPalette::Text));

	QRect textRect = rect();
	textRect.moveLeft(iconSize().width() + 4);
	painter.drawText(textRect, Qt::AlignVCenter | Qt::TextSingleLine, text());

	painter.drawPixmap(QPoint(2, (height() - iconSize().height()) / 2), icon().pixmap(iconSize()));

	QStyleOptionComboBox opt;
	opt.initFrom(this);
	opt.rect = QRect(width() - 15, height() / 2 - 6, 11, 13);
	painter.drawPrimitive(QStyle::PE_IndicatorArrowDown, opt);
}

void StatusChangeButton::setStatusOnline(bool signal)
{
	Core *core = Core::inst();

	setIcon(core->icon(KittySDK::Icons::I_STATUS_ONLINE));
	setText(core->statusToString(KittySDK::IProtocol::Online));

	m_status = KittySDK::IProtocol::Online;
	if(signal) {
		emit statusChanged(KittySDK::IProtocol::Online);
	}
}

void StatusChangeButton::setStatusAway(bool signal)
{
	Core *core = Core::inst();

	setIcon(core->icon(KittySDK::Icons::I_STATUS_AWAY));
	setText(core->statusToString(KittySDK::IProtocol::Away));

	m_status = KittySDK::IProtocol::Away;
	if(signal) {
		emit statusChanged(KittySDK::IProtocol::Away);
	}
}

void StatusChangeButton::setStatusFFC(bool signal)
{
	Core *core = Core::inst();

	setIcon(core->icon(KittySDK::Icons::I_STATUS_FFC));
	setText(core->statusToString(KittySDK::IProtocol::FFC));

	m_status = KittySDK::IProtocol::FFC;
	if(signal) {
		emit statusChanged(KittySDK::IProtocol::FFC);
	}
}

void StatusChangeButton::setStatusDND(bool signal)
{
	Core *core = Core::inst();

	setIcon(core->icon(KittySDK::Icons::I_STATUS_DND));
	setText(core->statusToString(KittySDK::IProtocol::DND));

	m_status = KittySDK::IProtocol::DND;
	if(signal) {
		emit statusChanged(KittySDK::IProtocol::DND);
	}
}

void StatusChangeButton::setStatusInvisible(bool signal)
{
	Core *core = Core::inst();

	setIcon(core->icon(KittySDK::Icons::I_STATUS_INVIS));
	setText(core->statusToString(KittySDK::IProtocol::Invisible));

	m_status = KittySDK::IProtocol::Invisible;
	if(signal) {
		emit statusChanged(KittySDK::IProtocol::Invisible);
	}
}

void StatusChangeButton::setStatusOffline(bool signal)
{
	Core *core = Core::inst();

	setIcon(core->icon(KittySDK::Icons::I_STATUS_OFFLINE));
	setText(core->statusToString(KittySDK::IProtocol::Offline));

	m_status = KittySDK::IProtocol::Offline;
	if(signal) {
		emit statusChanged(KittySDK::IProtocol::Offline);
	}
}

void StatusChangeButton::updateIcons()
{
	Core *core = Core::inst();

	m_onlineAction->setIcon(core->icon(KittySDK::Icons::I_STATUS_ONLINE));
	m_awayAction->setIcon(core->icon(KittySDK::Icons::I_STATUS_AWAY));
	m_FFCAction->setIcon(core->icon(KittySDK::Icons::I_STATUS_FFC));
	m_DNDAction->setIcon(core->icon(KittySDK::Icons::I_STATUS_DND));
	m_InvisibleAction->setIcon(core->icon(KittySDK::Icons::I_STATUS_INVIS));
	m_OfflineAction->setIcon(core->icon(KittySDK::Icons::I_STATUS_OFFLINE));

	setStatus(m_status);
}

}
