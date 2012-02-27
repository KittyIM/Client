#ifndef DEBUGWINDOW_H
#define DEBUGWINDOW_H

#include "Singleton.h"

#include <QtGui/QWidget>

class QTreeWidgetItem;
class QStringList;
class QWebView;

namespace Ui
{
	class DebugWindow;
}

namespace Kitty
{
	class DebugWindow: public QWidget, public Singleton<DebugWindow>
	{
		Q_OBJECT

		friend class Singleton<DebugWindow>;

		public:
			~DebugWindow();

			static void addMessage(QtMsgType type, const char *msg);

		protected:
			void showEvent(QShowEvent *event);
			void changeEvent(QEvent *event);
			bool eventFilter(QObject *obj, QEvent *event);

		private slots:
			void execCommand();
			void on_refreshActionsButton_clicked();
			void on_refreshIconsButton_clicked();
			void on_iconsWidget_currentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous);

		private:
			explicit DebugWindow();
			DebugWindow(const DebugWindow &);
			void operator=(const DebugWindow &);

		private:
			Ui::DebugWindow *m_ui;
			static DebugWindow* m_inst;
			QStringList m_history;
			static QWebView *m_wvLog;
	};
}

#endif // DEBUGWINDOW_H
