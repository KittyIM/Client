#ifndef WNDPROFILES_H
#define WNDPROFILES_H

#include <QtGui/QDialog>

class QTreeWidgetItem;

namespace Ui
{
  class wndProfiles;
}

class wndProfiles: public QDialog
{
  Q_OBJECT

  public:
    explicit wndProfiles(QWidget *parent = 0);
    ~wndProfiles();

  protected:
    void showEvent(QShowEvent *event);
    void keyPressEvent(QKeyEvent *event);
    void closeEvent(QCloseEvent *event);

  private slots:
    void on_profilesWidget_currentItemChanged(QTreeWidgetItem* current, QTreeWidgetItem* previous);

  private:
    Ui::wndProfiles *m_ui;
};

#endif // WNDPROFILES_H
