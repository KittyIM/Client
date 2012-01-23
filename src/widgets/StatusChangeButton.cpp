#include "StatusChangeButton.h"

#include "SDK/constants.h"
#include "IconManager.h"
#include "Core.h"

#include <QtCore/QDebug>
#include <QtGui/QStyleOptionToolButton>
#include <QtGui/QStylePainter>
#include <QtGui/QMenu>

using namespace Kitty;
using namespace KittySDK;

Kitty::StatusChangeButton::StatusChangeButton(QWidget *parent): QToolButton(parent)
{
	Core *core = Core::inst();
	QMenu *menu = new QMenu(this);

	connect(IconManager::inst(), SIGNAL(iconsUpdated()), this, SLOT(updateIcons()));

	m_onlineAction = menu->addAction(core->icon(Icons::I_STATUS_ONLINE), tr("Online"), this, SLOT(setStatusOnline()));
	m_awayAction = menu->addAction(core->icon(Icons::I_STATUS_AWAY), tr("Away"), this, SLOT(setStatusAway()));
	m_FFCAction = menu->addAction(core->icon(Icons::I_STATUS_FFC), tr("Free for chat"), this, SLOT(setStatusFFC()));
	m_DNDAction = menu->addAction(core->icon(Icons::I_STATUS_DND), tr("Do not disturb"), this, SLOT(setStatusDND()));
	m_InvisibleAction = menu->addAction(core->icon(Icons::I_STATUS_INVIS), tr("Invisible"), this, SLOT(setStatusInvisible()));
	m_OfflineAction = menu->addAction(core->icon(Icons::I_STATUS_OFFLINE), tr("Offline"), this, SLOT(setStatusOffline()));

	setMenu(menu);
	setPopupMode(QToolButton::InstantPopup);
	setIcon(core->icon(Icons::I_STATUS_OFFLINE));

	m_status = Protocol::Offline;
}

QSize Kitty::StatusChangeButton::sizeHint() const
{
	return QSize(2 + iconSize().width() + 2 + fontMetrics().width(text()) + 24, 20);
}

void Kitty::StatusChangeButton::retranslate()
{
	m_onlineAction->setText(tr("Online"));
	m_awayAction->setText(tr("Away"));
	m_FFCAction->setText(tr("Free for chat"));
	m_DNDAction->setText(tr("Do not disturb"));
	m_InvisibleAction->setText(tr("Invisible"));
	m_OfflineAction->setText(tr("Offline"));

	setStatus(m_status);
}

void Kitty::StatusChangeButton::setStatus(KittySDK::Protocol::Status status)
{
	switch(status) {
		case Protocol::Online:
			setStatusOnline(false);
		break;

		case Protocol::Away:
			setStatusAway(false);
		break;

		case Protocol::FFC:
			setStatusFFC(false);
		break;

		case Protocol::DND:
			setStatusDND(false);
		break;

		case Protocol::Invisible:
			setStatusInvisible(false);
		break;

		case Protocol::Offline:
			setStatusOffline(false);
		break;

		default:
			qWarning() << "Unknown status" << status;
		break;
	}
}

void Kitty::StatusChangeButton::paintEvent(QPaintEvent *event)
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

void Kitty::StatusChangeButton::setStatusOnline(bool signal)
{
	Core *core = Core::inst();

	setIcon(core->icon(Icons::I_STATUS_ONLINE));
	setText(core->statusToString(Protocol::Online));

	m_status = Protocol::Online;
	if(signal) {
		emit statusChanged(Protocol::Online);
	}
}

void Kitty::StatusChangeButton::setStatusAway(bool signal)
{
	Core *core = Core::inst();

	setIcon(core->icon(Icons::I_STATUS_AWAY));
	setText(core->statusToString(Protocol::Away));

	m_status = Protocol::Away;
	if(signal) {
		emit statusChanged(Protocol::Away);
	}
}

void Kitty::StatusChangeButton::setStatusFFC(bool signal)
{
	Core *core = Core::inst();

	setIcon(core->icon(Icons::I_STATUS_FFC));
	setText(core->statusToString(Protocol::FFC));

	m_status = Protocol::FFC;
	if(signal) {
		emit statusChanged(Protocol::FFC);
	}
}

void Kitty::StatusChangeButton::setStatusDND(bool signal)
{
	Core *core = Core::inst();

	setIcon(core->icon(Icons::I_STATUS_DND));
	setText(core->statusToString(Protocol::DND));

	m_status = Protocol::DND;
	if(signal) {
		emit statusChanged(Protocol::DND);
	}
}

void Kitty::StatusChangeButton::setStatusInvisible(bool signal)
{
	Core *core = Core::inst();

	setIcon(core->icon(Icons::I_STATUS_INVIS));
	setText(core->statusToString(Protocol::Invisible));

	m_status = Protocol::Invisible;
	if(signal) {
		emit statusChanged(Protocol::Invisible);
	}
}

void Kitty::StatusChangeButton::setStatusOffline(bool signal)
{
	Core *core = Core::inst();

	setIcon(core->icon(Icons::I_STATUS_OFFLINE));
	setText(core->statusToString(Protocol::Offline));

	m_status = Protocol::Offline;
	if(signal) {
		emit statusChanged(Protocol::Offline);
	}
}

void Kitty::StatusChangeButton::updateIcons()
{
	Core *core = Core::inst();

	m_onlineAction->setIcon(core->icon(Icons::I_STATUS_ONLINE));
	m_awayAction->setIcon(core->icon(Icons::I_STATUS_AWAY));
	m_FFCAction->setIcon(core->icon(Icons::I_STATUS_FFC));
	m_DNDAction->setIcon(core->icon(Icons::I_STATUS_DND));
	m_InvisibleAction->setIcon(core->icon(Icons::I_STATUS_INVIS));
	m_OfflineAction->setIcon(core->icon(Icons::I_STATUS_OFFLINE));

	setStatus(m_status);
}

