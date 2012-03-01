#ifndef PLUGINCOREIMPL_H
#define PLUGINCOREIMPL_H

#include <IPluginCore.h>

namespace KittySDK
{
	class ISettingsPage;
	class IAccount;
	class IContact;
}

namespace Kitty
{
	class PluginCoreImpl: public KittySDK::IPluginCore
	{
		Q_OBJECT

		public:
			PluginCoreImpl(QObject *parent = 0);

			QVariant setting(const QString &key, const QVariant &defaultValue = QVariant());
			void setSetting(const QString &key, const QVariant &value);

			void execPluginAction(const QString &pluginId, const QString &name, const QMap<QString, QVariant> &args);

			QString profileName();
			QString kittyDir();
			QString profilesDir();
			QString avatarPath(KittySDK::IContact *contact);

			int contactCount();
			KittySDK::IContact *contact(const int &id);
			QList<KittySDK::IContact*> contacts(const QString &account, const QString &protocol);
			QList<KittySDK::IContact*> contacts(const QString &protocol);

			QStringList plugins();

			QVariant jsonParse(const QString &json);
			QString jsonStringify(const QVariant &json, int indent);

			void addSettingPage(KittySDK::ISettingsPage *page, const QString &parent);
			void removeSettingPage(KittySDK::ISettingsPage *page);

			void addToolbarAction(const QString &tb, QAction *action);
			void removeToolbarAction(const QString &tb, QAction *action);
			QToolButton *buttonForAction(const QString &tb, QAction *action);

			void addAccount(KittySDK::IAccount *account);

			QAction *action(const QString &id);
			void addAction(const QString &id, QAction *action);
			void removeAction(const QString &id);

			QPixmap icon(const QString &id);
			void addIcon(const QString &id, const QPixmap &pixmap, bool replace = true);
			void removeIcon(const QString &id);

			quint32 enqueue(const KittySDK::IMessage &msg);
			void dequeue(const quint32 &msgId);
			void dequeue(const QString &chatId);
			quint32 queueMsgId(const KittySDK::IMessage &msg);

			virtual QString kittyVersion();
	};
}

#endif // PLUGINCOREIMPL_H
