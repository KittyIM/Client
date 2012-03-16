#include "RosterItemDelegate.h"

#include "RosterItem.h"
#include "Core.h"

#include <SDKConstants.h>

#include <QtCore/QDebug>
#include <QtGui/QApplication>
#include <QtGui/QPainter>

#define qDebug() qDebug() << "[RosterItemDelegate]"
#define qWarning() qWarning() << "[RosterItemDelegate]"

namespace Kitty
{

RosterItemDelegate::RosterItemDelegate(QObject *parent):
	QStyledItemDelegate(parent)
{
}

void RosterItemDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
	int type = index.data(RosterItem::TypeRole).toInt();
	int avatarPos = Core::inst()->setting(KittySDK::Settings::S_ROSTER_AVATARS).toInt();
	int descriptionPos = Core::inst()->setting(KittySDK::Settings::S_ROSTER_STATUS_DESCRIPTION).toInt();

	if(descriptionPos < 2) {
		avatarPos = 0;
	}

	painter->save();

	QStyleOptionViewItemV4 vopt = option;
	initStyleOption(&vopt, index);

	const QWidget *widget = vopt.widget;
	QStyle *style = widget ? widget->style() : QApplication::style();

	QRect iconRect = style->subElementRect(QStyle::SE_ItemViewItemDecoration, &vopt, vopt.widget);
	QRect textRect = style->subElementRect(QStyle::SE_ItemViewItemText, &vopt, vopt.widget);

	style->drawPrimitive(QStyle::PE_PanelItemViewItem, &vopt, painter, vopt.widget);

	if(type == RosterItem::Group) {
		QStyleOption opt = option;
		opt.rect = QRect(2, textRect.top() + textRect.height() / 2 - 6, 15, 15);

		if(option.state & QStyle::State_Open) {
			style->drawPrimitive(QStyle::PE_IndicatorArrowDown, &opt, painter);
		} else {
			style->drawPrimitive(QStyle::PE_IndicatorArrowRight, &opt, painter);
		}
	} else {
		vopt.icon.paint(painter, iconRect, vopt.decorationAlignment, QIcon::Normal, QIcon::On);

		if(index.data(RosterItem::TemporaryRole).toBool()) {
			painter->drawPixmap(iconRect.adjusted(0, 8, -8, 0), Core::inst()->icon(KittySDK::Icons::I_DELETE));
		}
	}

	QPalette::ColorGroup cg = (vopt.state & QStyle::State_Enabled) ? QPalette::Normal : QPalette::Disabled;
	if((cg == QPalette::Normal) && !(vopt.state & QStyle::State_Active)) {
		cg = QPalette::Inactive;
	}

	if(vopt.state & QStyle::State_Selected) {
		painter->setPen(vopt.palette.color(cg, QPalette::HighlightedText));
	} else {
		painter->setPen(vopt.palette.color(cg, QPalette::Text));
	}

	QFont normalFont = vopt.font;
	QFont boldFont = vopt.font;
	boldFont.setBold(true);

	if(type == RosterItem::Group) {
		painter->setFont(boldFont);
		painter->drawText(textRect.translated(17, 0), Qt::AlignVCenter | Qt::TextSingleLine, vopt.text);
		painter->setFont(normalFont);
	} else {
		QString description;
		if(Core::inst()->setting(KittySDK::Settings::S_ROSTER_STATUS_DESCRIPTION, true).toBool()) {
			description = index.data(RosterItem::DescriptionRole).toString();
		}

		//avatarPos = 0 means avatar disabled
		if(avatarPos > 0) {
			QPixmap avatar(index.data(RosterItem::AvatarRole).toString());
			if(!avatar.isNull()) {
				if(avatar.size() != QSize(32, 32)) {
					avatar = avatar.scaled(32, 32, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
				}

				//avatar on the left
				if(avatarPos == 1) {
					painter->drawPixmap(textRect.x(), vopt.rect.top() + 2, 32, 32, avatar);
					textRect.adjust(34, 0, 0, 0);
				} else if(avatarPos == 2) {
				//avatar on the right
					textRect.adjust(0, 0, -34, 0);
					painter->drawPixmap(vopt.rect.width() - 34, vopt.rect.top() + 2, 32, 32, avatar);
				}
			}
		}

		if((descriptionPos <= 1) || ((descriptionPos == 2) && description.isEmpty())) {
			painter->setFont(boldFont);
			painter->drawText(textRect, Qt::AlignVCenter | Qt::TextSingleLine, painter->fontMetrics().elidedText(vopt.text, Qt::ElideRight, textRect.width()));
			painter->setFont(normalFont);
		}

		if(!description.isEmpty()) {
			//description on the right
			if(descriptionPos == 1) {
				painter->setFont(boldFont);
				int width = textRect.width() - painter->fontMetrics().width(vopt.text) - 10;
				painter->setFont(normalFont);

				if(width > 0) {
					QFont font = normalFont;
					font.setPointSize(font.pointSize() - 2);
					painter->setFont(font);

					painter->drawText(QRect(textRect.x() + (textRect.width() - width), textRect.top(), width - 5, textRect.height()), Qt::AlignVCenter | Qt::AlignRight | Qt::TextSingleLine, painter->fontMetrics().elidedText(description, Qt::ElideRight, width - 5));
				}
			} else if(descriptionPos == 2) {
			//description under name
				QRect rect = textRect;
				rect.translate(0, 3);
				rect.setHeight(16);
				painter->setFont(boldFont);
				painter->drawText(rect, Qt::TextSingleLine, painter->fontMetrics().elidedText(vopt.text, Qt::ElideRight, textRect.width()));
				painter->setFont(normalFont);

				QFont descFont = normalFont;
				descFont.setPointSize(descFont.pointSize() - 1);
				painter->setFont(descFont);

				rect = textRect;
				rect.translate(0, rect.height() / 2 + 1);
				rect.setHeight(18);
				painter->drawText(rect, Qt::TextSingleLine, painter->fontMetrics().elidedText(description, Qt::ElideRight, textRect.width()));
			}
		}

		//a line over the first contact without a group
		if(!index.parent().isValid()) {
			QModelIndex prev = index.sibling(index.row() - 1, 0);
			if(prev.isValid()) {
				if(prev.data(RosterItem::TypeRole).toInt() == RosterItem::Group) {

					painter->setPen(vopt.palette.color(cg, QPalette::Mid));
					painter->drawLine(option.rect.left(), option.rect.top(), option.rect.width(), option.rect.top());
				}
			}
		}
	}

	painter->restore();
}

QSize RosterItemDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
	int height = 26;

	if(index.data(RosterItem::TypeRole).toInt() == RosterItem::Contact) {
		int descriptionPos = Core::inst()->setting(KittySDK::Settings::S_ROSTER_STATUS_DESCRIPTION).toInt();
		if(descriptionPos <= 1) {
			height = 20;
		} else if(descriptionPos == 2) {
			height = 36;
		}
	}

	return QSize(option.rect.width(), height);
}

}
