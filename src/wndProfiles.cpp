#include "wndProfiles.h"
#include "ui_wndProfiles.h"

#include "constants.h"
#include "qtwin/qtwin.h"
#include "KittyProfile.h"

#include <QtCore/QDir>
#include <QtCore/QDebug>
#include <QtGui/QKeyEvent>

wndProfiles::wndProfiles(QWidget *parent): QDialog(parent), m_ui(new Ui::wndProfiles)
{
  m_ui->setupUi(this);
  m_ui->versionLabel->setText(QString("KittyIM v%1").arg(Constants::VERSION));

  if(QtWin::isCompositionEnabled()) {
    //QtWin::extendFrameIntoClientArea(this);
    //setContentsMargins(0, 0, 0, 0);
  }
}

wndProfiles::~wndProfiles()
{
  delete m_ui;
}

void wndProfiles::showEvent(QShowEvent *event)
{
  QDialog::showEvent(event);

  m_ui->passwordEdit->clear();
  m_ui->passwordEdit->hide();
  m_ui->passwordLabel->hide();

  m_ui->profilesWidget->clear();

  QDir dir(qApp->applicationDirPath() + "/profiles");
  QFileInfoList profiles = dir.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot, QDir::Name);
  foreach(QFileInfo info, profiles) {
    QTreeWidgetItem *item = new QTreeWidgetItem();
    item->setText(0, info.baseName());
    item->setIcon(0, QIcon(":/icons/main.ico"));

    if(QFile::exists(info.absoluteFilePath() + "/avatar.png")) {
      item->setIcon(0, QIcon(info.absoluteFilePath() + "/avatar.png"));
    }

    m_ui->profilesWidget->addTopLevelItem(item);
  }
}

void wndProfiles::keyPressEvent(QKeyEvent *event)
{
  if(event->key() == Qt::Key_Escape) {
    event->ignore();
  } else {
    QDialog::keyReleaseEvent(event);
  }
}

void wndProfiles::closeEvent(QCloseEvent *event)
{
  QDialog::closeEvent(event);
  qApp->quit();
}

void wndProfiles::on_profilesWidget_currentItemChanged(QTreeWidgetItem* current, QTreeWidgetItem* previous)
{
  if(current) {
    KittyProfile pro(this);
    pro.load(current->text(0));

    m_ui->passwordEdit->clear();
    m_ui->passwordEdit->setVisible(pro.hasPassword());
    m_ui->passwordLabel->setVisible(pro.hasPassword());
  }
}
