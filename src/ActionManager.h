#ifndef ACTIONMANAGER_H
#define ACTIONMANAGER_H

#include <QtCore/QStringList>
#include <QtCore/QPointer>
#include <QtCore/QHash>
#include <QtGui/QAction>

namespace Kitty
{
	class Core;

	class ActionManager: public QObject
	{
		Q_OBJECT

		public:
			ActionManager(Core *core);
			~ActionManager();

			void loadDefaults();
			QStringList keys() const { return m_actions.keys(); }

			void insert(const QString &id, QAction *action);
			QAction *action(const QString &id) const { return m_actions.value(id); }

		public slots:
			void updateIcons();
			void retranslate();

		private:
			QHash<QString, QPointer<QAction> > m_actions;
			Core *m_core;
	};
}
#endif // ACTIONMANAGER_H
