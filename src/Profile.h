#ifndef PROFILE_H
#define PROFILE_H

#include <QtCore/QObject>
#include <QtCore/QString>

namespace Kitty
{
	class JsonSettings;

	class Profile: public QObject
	{
		Q_OBJECT

		public:
			explicit Profile(QObject *parent = 0);
			~Profile();

			void load(const QString &name);
			void loadIconTheme(const QString &name);

			QString name() const { return m_name; }
			JsonSettings *settings() const { return m_settings; }
			bool isLoaded() const { return !m_name.isEmpty(); }

		private:
			QString m_name;
			JsonSettings *m_settings;
	};
}

#endif // PROFILE_H
