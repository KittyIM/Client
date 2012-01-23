#ifndef CORE_H
#define CORE_H

#include "Singleton.h"

#include <QtCore/QPointer>
#include <QtCore/QVariant>
#include <QtCore/QObject>
#include <QtGui/QSystemTrayIcon>

class QAction;
class Hunspell;

namespace KittySDK
{
	class Contact;
	class Message;
}

namespace Kitty
{
	class ProfilesWindow;
	class SettingsWindow;
	class ContactWindow;
	class HistoryWindow;
	class JsonSettings;
	class AboutWindow;
	class ChatWindow;
	class MainWindow;
	class Profile;

	class Core: public QObject, public Singleton<Core>
	{
		Q_OBJECT
		Q_PROPERTY(bool restart READ hasToRestart WRITE setRestart)
		Q_PROPERTY(bool portable READ isPortable WRITE setPortable)
		Q_PROPERTY(QStringList appArguments READ appArguments WRITE setAppArguments)

		friend class Singleton<Core>;

		public:
			~Core();

			QString statusToString(const int &status);
			QString processUrls(const QString &text);
			QString processEmoticons(const QString &text);

			QAction *action(const QString &id) const;
			QPixmap icon(const QString &id) const;
			QVariant setting(const QString &key, const QVariant &defaultValue = QVariant());

			void setSetting(const QString &key, const QVariant &value);
			void loadProfile(const QString &name);

			MainWindow *mainWindow();
			ChatWindow *chatWindow();
			AboutWindow *aboutWindow();
			ProfilesWindow *profilesWindow();
			SettingsWindow *settingsWindow();
			HistoryWindow *historyWindow();
			QSystemTrayIcon *trayIcon();
			Profile *profile();
			Hunspell *hunspell();
			JsonSettings *settings();

			QString avatarPath(KittySDK::Contact *contact);
			QString profilesDir() const;
			QString currentProfileDir();
			bool removeDir(const QString &dirName);
			bool archiveMessage(const KittySDK::Message &msg);

			void setRestart(bool restart) { m_restart = restart; }
			bool hasToRestart() const { return m_restart; }

			void setPortable(bool portable) { m_portable = portable; }
			bool isPortable() const { return m_portable; }

			void setAppArguments(const QStringList &appArguments) { m_appArguments = appArguments; }
			QStringList appArguments() const { return m_appArguments; }

		public slots:
			void restart();
			void changeProfile(const QString &profile, const QString &password);
			void showTrayIcon();
			void showAboutWindow();
			void showMainWindow();
			void showChatWindow();
			void showProfilesWindow();
			void showSettingsWindow();
			void showHistoryWindow();
			void showAddContactWindow();
			void toggleMainWindow();
			void openKittyFolder();
			void openProfilesFolder();
			void showContactWindow(KittySDK::Contact *cnt);

		private slots:
			void trayIconActivated(QSystemTrayIcon::ActivationReason reason);

		private:
			Core();
			Core(const Core &);
			void operator=(const Core &);

		private:
			bool m_restart;
			bool m_portable;
			QStringList m_appArguments;
			QMap<QString, QPointer<ContactWindow> > m_contactWindows;
			QPointer<ProfilesWindow> m_profilesWindow;
			QPointer<SettingsWindow> m_settingsWindow;
			QPointer<HistoryWindow> m_historyWindow;
			QPointer<QSystemTrayIcon> m_trayIcon;
			QPointer<AboutWindow> m_aboutWindow;
			QPointer<MainWindow> m_mainWindow;
			QPointer<ChatWindow> m_chatWindow;
			QPointer<Profile> m_profile;
			Hunspell *m_hunspell;
	};
}

#endif // CORE_H
