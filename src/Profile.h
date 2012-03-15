#ifndef PROFILE_H
#define PROFILE_H

#include <QtCore/QObject>
#include <QtCore/QString>

namespace Kitty
{
	class JsonSettings;
	class Core;

	class Profile: public QObject
	{
		Q_OBJECT

		public:
			Profile(Core *core);
			~Profile();

			void load(const QString &name);
			void loadIconTheme(const QString &name);

			QString name() const { return m_name; }
			JsonSettings *settings() const { return m_settings; }
			bool isLoaded() const { return !m_name.isEmpty(); }

		private:
			QString m_name;
			Core *m_core;
			JsonSettings *m_settings;
	};
}

#endif // PROFILE_H
