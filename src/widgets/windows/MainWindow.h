#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <IProtocol.h>

#include <QtCore/QTimer>
#include <QtGui/QSystemTrayIcon>
#include <QtGui/QMainWindow>

class QToolButton;

namespace Ui
{
	class MainWindow;
}

namespace KittySDK
{
	class IContact;
}

namespace Kitty
{
	class RosterItemModel;
	class RosterSortProxy;
	class RosterHeader;

	class MainWindow: public QMainWindow
	{
		Q_OBJECT

		public:
			explicit MainWindow(QWidget *parent = 0);
			~MainWindow();

			bool isObscured();

			void initToolbars();
			void addToolbarAction(const QString &tb, QAction *action);

			QToolButton *buttonForAction(const QString &tb, QAction *action);

		public slots:
			void addContact(KittySDK::IContact *contact);
			void applySettings();
			void showAccountStatusMenu();
			void hideFilterEdit();
			void updateAccountStatusIcon(KittySDK::IAccount *account, KittySDK::IProtocol::Status status, const QString &description);

		private slots:
			void showToolbarMenu(QPoint pos);
			void setToolbarIconsOnly();
			void setToolbarTextOnly();
			void setToolbarTextBeside();
			void setToolbarTextUnder();
			void setFilterText(const QString &text);
			void toggleToolbarLock();
			void requestHistory(KittySDK::IContact *contact);
			void trayIconActivated(QSystemTrayIcon::ActivationReason reason);
			void resetTrayIcon();
			void maybeSetupBlink(const quint32 &msgId);
			void blinkTrayIcon();
			void maybeUnblink(const quint32 &msgId);

		protected:
			void changeEvent(QEvent *event);
			void enterEvent(QEvent *event);
			void leaveEvent(QEvent *event);
			bool eventFilter(QObject *obj, QEvent *event);

		private:
			QTimer m_hideTimer;
			QTimer m_blinkTimer;
			QList<quint32> m_blinkQueue;
			Ui::MainWindow *m_ui;
			QSystemTrayIcon *m_trayIcon;
			RosterHeader *m_header;
			RosterItemModel *m_model;
			RosterSortProxy *m_proxy;
	};
}

#endif // MAINWINDOW_H
