#ifndef ICONTHEME_H
#define ICONTHEME_H

#include "AbstractTheme.h"

#include <QtCore/QHash>

namespace Kitty
{
	class IconTheme: public AbstractTheme
	{
		public:
			IconTheme(const QString &name = "");

			QHash<QString, QString> icons() const { return m_icons; }

		private:
			void loadDefaults();

		private:
			QHash<QString, QString> m_icons;
	};
}

#endif // ICONTHEME_H
