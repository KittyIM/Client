#ifndef SETTINGSWINDOW_H
#define SETTINGSWINDOW_H

#include <QtGui/QDialog>

class QTreeWidgetItem;

namespace Ui
{
  class SettingsWindow;
}

namespace Kitty
{
  class SettingsWindow: public QDialog
  {
    Q_OBJECT

    public:
      explicit SettingsWindow(QWidget *parent = 0);
      ~SettingsWindow();

      QTreeWidgetItem *itemById(const QString &id);

      void updateIcons();

    signals:
      void settingsApplied();

    private slots:
      void on_treeWidget_currentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous);

    private:
      Ui::SettingsWindow *m_ui;
  };
}

#endif // SETTINGSWINDOW_H
