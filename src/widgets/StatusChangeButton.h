#ifndef STATUSCHANGEBUTTON_H
#define STATUSCHANGEBUTTON_H

#include <IProtocol.h>

#include <QtGui/QToolButton>

namespace Kitty
{
	class StatusChangeButton: public QToolButton
	{
		Q_OBJECT

		public:
			explicit StatusChangeButton(QWidget *parent = 0);

			QSize sizeHint() const;

			KittySDK::IProtocol::Status status() const { return m_status; }

		public slots:
			void setStatus(KittySDK::IProtocol::Status status);
			void retranslate();

		signals:
			void statusChanged(KittySDK::IProtocol::Status status);

		private slots:
			void setStatusOnline(bool signal = true);
			void setStatusAway(bool signal = true);
			void setStatusFFC(bool signal = true);
			void setStatusDND(bool signal = true);
			void setStatusInvisible(bool signal = true);
			void setStatusOffline(bool signal = true);
			void updateIcons();

		protected:
			void paintEvent(QPaintEvent *event);

		private:
			KittySDK::IProtocol::Status m_status;
			QAction *m_onlineAction;
			QAction *m_awayAction;
			QAction *m_FFCAction;
			QAction *m_DNDAction;
			QAction *m_InvisibleAction;
			QAction *m_OfflineAction;

	};
}

#endif
