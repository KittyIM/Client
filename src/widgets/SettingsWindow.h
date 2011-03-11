#ifndef SETTINGSWINDOW_H
#define SETTINGSWINDOW_H

#include <QtGui/QDialog>

class QTreeWidgetItem;

namespace Ui
{
  class SettingsWindow;
}

namespace KittySDK
{
  class SettingPage;
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
      void resetSettings();

    signals:
      void settingsApplied();

    protected:
      void showEvent(QShowEvent *event);

    private slots:
      void on_treeWidget_currentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous);
      void applySettings();

      void on_buttonBox_accepted();

      void on_buttonBox_rejected();

  private:
      Ui::SettingsWindow *m_ui;
      QList<KittySDK::SettingPage*> m_pages;
  };
}

#endif // SETTINGSWINDOW_H
