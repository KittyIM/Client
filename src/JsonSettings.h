#ifndef JSONSETTINGS_H
#define JSONSETTINGS_H

#include <QtCore/QSettings>

class QDomElement;

namespace Kitty
{
	class JsonSettings: public QSettings
	{
		Q_OBJECT

		public:
			explicit JsonSettings(const QString &fileName, QObject *parent = 0);

			static void readMap(SettingsMap &map, const QVariant &root, const QString &name);
			static void writeMap(QVariant &root, const QString &key, const QVariant &value);

			static bool jsonRead(QIODevice &device, SettingsMap &map);
			static bool jsonWrite(QIODevice &device, const SettingsMap &map);

			static QString variantToString(const QVariant &v);
			static QVariant stringToVariant(const QString &s);
			static QStringList splitArgs(const QString &s, int idx);

		public:
			static QSettings::Format jsonFormat;
	};
}

#endif // JSONSETTINGS_H
