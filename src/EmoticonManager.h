#ifndef EMOTICONMANAGER_H
#define EMOTICONMANAGER_H

#include "Singleton.h"

#include <QtCore/QStringList>

namespace Kitty
{
	class EmoticonPack;

	class Emoticon
	{
		public:
			Emoticon(const QString &fileName = "", const QStringList &texts = QStringList()): m_fileName(fileName), m_texts(texts) { }
			Emoticon(const QString &fileName, const QString &text): m_fileName(fileName), m_texts(QStringList() << text)  { }

			QString fileName() const { return m_fileName; }
			void setFileName(const QString &fileName) { m_fileName = fileName; }

			QStringList texts() const { return m_texts; }
			void setTexts(const QStringList &texts) { m_texts = texts; }

			void addText(const QString &text) { m_texts.append(text); }

		private:
			QString m_fileName;
			QStringList m_texts;
	};

	class EmoticonManager: public QObject, public Singleton<EmoticonManager>
	{
		Q_OBJECT

		friend class Singleton<EmoticonManager>;

		public:
			QString parse(const QString &text);

		public slots:
			void load();
			void applySettings();

		private:
			EmoticonManager(QObject *parent = 0): QObject(parent) { }
			~EmoticonManager();

		private:
			QList<EmoticonPack*> m_packs;

	};
}

#endif // EMOTICONMANAGER_H
