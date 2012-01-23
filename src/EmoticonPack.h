#ifndef EMOTICONPACK_H
#define EMOTICONPACK_H

#include <QtCore/QObject>

namespace Kitty
{
	class Emoticon;

	class EmoticonPack: public QObject
	{
		Q_OBJECT

		public:
			explicit EmoticonPack(const QString &name, QObject *parent = 0);
			~EmoticonPack();

			QString name() const { return m_name; }
			const QList<Emoticon*> &emoticons() const { return m_emots; }

		private:
			QString m_name;
			QList<Emoticon*> m_emots;
	};
}

#endif // EMOTICONPACK_H
