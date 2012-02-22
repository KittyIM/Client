#ifndef ACTIONMANAGER_H
#define ACTIONMANAGER_H

#include "Singleton.h"

#include <QtCore/QStringList>
#include <QtCore/QPointer>
#include <QtCore/QHash>
#include <QtGui/QAction>

namespace Kitty
{
	class ActionManager: public QObject, public Singleton<ActionManager>
	{
		Q_OBJECT

		friend class Singleton<ActionManager>;

		public:
			void loadDefaults();
			QStringList keys() const { return m_actions.keys(); }

			void insert(const QString &id, QAction *action);
			QAction *action(const QString &id) const { return m_actions.value(id); }

		public slots:
			void updateIcons();
			void retranslate();

		private:
			ActionManager(): QObject(0) { }

		private:
			QHash<QString, QPointer<QAction> > m_actions;
	};
}
#endif // ACTIONMANAGER_H
