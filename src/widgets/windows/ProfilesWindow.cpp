#include "ProfilesWindow.h"
#include "ui_ProfilesWindow.h"

#include "3rdparty/qtwin/qtwin.h"
#include "SDK/constants.h"
#include "JsonSettings.h"
#include "constants.h"
#include "Profile.h"
#include "Core.h"

#include <QtCore/QCryptographicHash>
#include <QtCore/QDebug>
#include <QtCore/QDir>
#include <QtGui/QInputDialog>
#include <QtGui/QMessageBox>
#include <QtGui/QKeyEvent>
#include <QtGui/QPainter>

#define qDebug() qDebug() << "[ProfilesWindow]"
#define qWarning() qWarning() << "[ProfilesWindow]"

using namespace Kitty;
using namespace KittySDK;

Kitty::ProfilesWindow::ProfilesWindow(QWidget *parent): QDialog(parent), m_ui(new Ui::ProfilesWindow)
{
  m_ui->setupUi(this);
  m_ui->versionLabel->setText(QString("KittyIM v%1").arg(Constants::VERSION));

  setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);

  qDebug() << "Creating";

  if(QtWin::isCompositionEnabled()) {
    QtWin::extendFrameIntoClientArea(this);
    setContentsMargins(0, 0, 0, 0);
  }
}

Kitty::ProfilesWindow::~ProfilesWindow()
{
  delete m_ui;
}

void Kitty::ProfilesWindow::showEvent(QShowEvent *event)
{
  QDialog::showEvent(event);

  m_ui->passwordEdit->clear();
  m_ui->passwordEdit->hide();
  m_ui->passwordLabel->hide();

  m_ui->profilesWidget->clear();

  QDir dir(Core::inst()->profilesDir());
  if(dir.exists()) {
    QFileInfoList profiles = dir.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot, QDir::Name | QDir::IgnoreCase);
    foreach(QFileInfo info, profiles) {
      QTreeWidgetItem *item = new QTreeWidgetItem(m_ui->profilesWidget);
      item->setText(0, info.baseName());
      item->setIcon(0, QIcon(":/icons/main.ico"));

      if(QFile::exists(info.absoluteFilePath() + "/avatar.png")) {
        item->setIcon(0, QIcon(info.absoluteFilePath() + "/avatar.png"));
      }
    }
  }

  QTreeWidgetItem *item = new QTreeWidgetItem(m_ui->profilesWidget);
  item->setText(0, tr("Add new profile"));
  item->setIcon(0, QIcon(":/icons/main.ico"));
  item->setData(0, Qt::UserRole, true);
}

void Kitty::ProfilesWindow::paintEvent(QPaintEvent *event)
{
#ifdef Q_WS_WIN32
  if(QtWin::isCompositionEnabled()) {
    QPainter p(this);
    p.setPen(palette().midlight().color());
    p.setBrush(palette().window());
    p.drawRoundedRect(m_ui->groupBox->geometry(), 2, 2);
  }
#endif

  QDialog::paintEvent(event);
}

void Kitty::ProfilesWindow::keyPressEvent(QKeyEvent *event)
{
  if(event->key() == Qt::Key_Escape) {
    event->ignore();
  } else {
    QDialog::keyReleaseEvent(event);
  }
}

void Kitty::ProfilesWindow::closeEvent(QCloseEvent *event)
{
  if(!Core::inst()->profile()->isLoaded()) {
    qApp->quit();
  }

  QDialog::closeEvent(event);
}

void Kitty::ProfilesWindow::on_profilesWidget_currentItemChanged(QTreeWidgetItem* current, QTreeWidgetItem* previous)
{
  m_ui->loginButton->setEnabled(false);
  m_ui->deleteButton->setEnabled(false);

  if(current) {
    if(current->data(0, Qt::UserRole).toBool() == false) {
      JsonSettings set(Core::inst()->profilesDir() + current->text(0) + "/settings.dat");
      bool hasPassword = !set.value(Settings::S_PROFILE_PASSWORD).toString().isEmpty();

      m_ui->passwordEdit->clear();
      m_ui->passwordEdit->setVisible(hasPassword);
      m_ui->passwordLabel->setVisible(hasPassword);

      m_ui->loginButton->setEnabled(true);
      m_ui->deleteButton->setEnabled(true);
    }
  }
}

void Kitty::ProfilesWindow::on_profilesWidget_itemDoubleClicked(QTreeWidgetItem *item, int column)
{
  Q_UNUSED(column)

  if(item) {
    if(item->data(0, Qt::UserRole).toBool() == false) {
      JsonSettings set(Core::inst()->profilesDir() + item->text(0) + "/settings.dat");
      bool hasPassword = !set.value(Settings::S_PROFILE_PASSWORD).toString().isEmpty();

      if(!hasPassword || (hasPassword && set.value(Settings::S_PROFILE_PASSWORD).toString() == QCryptographicHash::hash(m_ui->passwordEdit->text().toLocal8Bit(), QCryptographicHash::Sha1).toHex())) {
        Core::inst()->loadProfile(item->text(0));
        close();
      } else {
        QMessageBox::information(this, tr("Wrong password"), tr("The password you supplied is wrong."));
      }
    } else {
      QString profile = QInputDialog::getText(this, tr("New profile"), tr("Please input a name for the new profile:"));
      if(!profile.isEmpty()) {
        QDir dir(Core::inst()->profilesDir());
        if(!dir.exists(profile)) {
          if(dir.mkdir(profile)) {
            showEvent(new QShowEvent());
          } else {
            QMessageBox::warning(this, tr("Error"), tr("There was a problem creating your profile."));
          }
        } else {
          QMessageBox::warning(this, tr("Profile exists"), tr("Profile with this name already exists."));
        }
      }
    }
  }
}

void Kitty::ProfilesWindow::on_loginButton_clicked()
{
  on_profilesWidget_itemDoubleClicked(m_ui->profilesWidget->currentItem(), 0);
}

void Kitty::ProfilesWindow::on_deleteButton_clicked()
{
  QTreeWidgetItem *item = m_ui->profilesWidget->currentItem();
  if(item) {
    QString profile = item->text(0);

    if(m_ui->passwordEdit->isVisible()) {
      JsonSettings set(Core::inst()->profilesDir() + profile + "/settings.dat");
      if(set.value(Settings::S_PROFILE_PASSWORD).toString() != QCryptographicHash::hash(m_ui->passwordEdit->text().toLocal8Bit(), QCryptographicHash::Sha1).toHex()) {
        QMessageBox::information(this, tr("Error"), tr("Please enter the password to delete this profile."));
        return;
      }
    }

    if(QMessageBox::question(this, tr("Delete profile"), tr("Do you really want to delete this profile?"), QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes) {
      Core::inst()->removeDir(Core::inst()->profilesDir() + profile);
      showEvent(new QShowEvent());
    }
  }
}
