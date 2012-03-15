#ifndef EMOTICONMANAGER_H
#define EMOTICONMANAGER_H

#include <QtCore/QStringList>

namespace Kitty
{
	class EmoticonPack;
	class Core;

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

	class EmoticonManager: public QObject
	{
		Q_OBJECT

		public:
			EmoticonManager(Core *core);
			~EmoticonManager();

			QString parse(const QString &text);

		public slots:
			void load();
			void applySettings();

		private:
			QList<EmoticonPack*> m_packs;
			Core *m_core;
	};
}

#endif // EMOTICONMANAGER_H
