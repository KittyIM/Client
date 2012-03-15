#ifndef CORE_H
#define CORE_H

#include "Singleton.h"

#include <QtCore/QStringList>
#include <QtCore/QPointer>
#include <QtCore/QVariant>

class QAction;
class Hunspell;

namespace KittySDK
{
	class IContact;
	class IMessage;
}

namespace Kitty
{
	class EmoticonManager;
	class ProtocolManager;
	class AccountManager;
	class ContactManager;
	class ProfilesWindow;
	class SettingsWindow;
	class ActionManager;
	class ContactWindow;
	class HistoryWindow;
	class PluginManager;
	class JsonSettings;
	class MessageQueue;
	class AboutWindow;
	class ChatManager;
	class DebugWindow;
	class IconManager;
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

			QAction *action(const QString &id);
			QPixmap icon(const QString &id);
			QVariant setting(const QString &key, const QVariant &defaultValue = QVariant());

			void setSetting(const QString &key, const QVariant &value);
			void loadProfile(const QString &name);

			MainWindow *mainWindow();
			ChatWindow *chatWindow();
			AboutWindow *aboutWindow();
			ProfilesWindow *profilesWindow();
			SettingsWindow *settingsWindow();
			HistoryWindow *historyWindow();
			Profile *profile();
			Hunspell *hunspell();
			JsonSettings *settings();

			AccountManager *accountManager();
			ChatManager *chatManager();
			ActionManager *actionManager();
			ContactManager *contactManager();
			EmoticonManager *emoticonManager();
			IconManager *iconManager();
			MessageQueue *messageQueue();
			PluginManager *pluginManager();
			ProtocolManager *protocolManager();
			DebugWindow *debugWindow();

			QString avatarPath(KittySDK::IContact *contact) const;
			QString profileName() const;
			QString profilesDir() const;
			QString currentProfileDir() const;
			bool removeDir(const QString &dirName);
			void archiveMessage(const KittySDK::IMessage &msg);

			void setRestart(bool restart) { m_restart = restart; }
			bool hasToRestart() const { return m_restart; }

			void setPortable(bool portable) { m_portable = portable; }
			bool isPortable() const { return m_portable; }

			void setAppArguments(const QStringList &appArguments) { m_appArguments = appArguments; }
			QStringList appArguments() const { return m_appArguments; }

		public slots:
			void restart();
			void changeProfile(const QString &profile, const QString &password);
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
			void showContactWindow(KittySDK::IContact *cnt);

		private:
			Core();

		private:
			bool m_restart;
			bool m_portable;
			QStringList m_appArguments;
			QMap<QString, QPointer<ContactWindow> > m_contactWindows;
			QPointer<EmoticonManager> m_emoticonManager;
			QPointer<ProtocolManager> m_protocolManager;
			QPointer<AccountManager> m_accountManager;
			QPointer<ContactManager> m_contactManager;
			QPointer<ProfilesWindow> m_profilesWindow;
			QPointer<SettingsWindow> m_settingsWindow;
			QPointer<ActionManager> m_actionManager;
			QPointer<PluginManager> m_pluginManager;
			QPointer<HistoryWindow> m_historyWindow;
			QPointer<MessageQueue> m_messageQueue;
			QPointer<AboutWindow> m_aboutWindow;
			QPointer<ChatManager> m_chatManager;
			QPointer<DebugWindow> m_debugWindow;
			QPointer<IconManager> m_iconManager;
			QPointer<MainWindow> m_mainWindow;
			QPointer<ChatWindow> m_chatWindow;
			QPointer<Profile> m_profile;
			Hunspell *m_hunspell;
	};
}

#endif // CORE_H
