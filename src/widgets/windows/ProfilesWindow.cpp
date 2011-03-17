#include "ProfilesWindow.h"
#include "ui_ProfilesWindow.h"

#include "SDK/constants.h"
#include "qtwin/qtwin.h"
#include "XmlSettings.h"
#include "constants.h"
#include "Profile.h"
#include "Core.h"

#include <QtCore/QCryptographicHash>
#include <QtCore/QDebug>
#include <QtCore/QDir>
#include <QtGui/QMessageBox>
#include <QtGui/QKeyEvent>
#include <QtGui/QPainter>

Kitty::ProfilesWindow::ProfilesWindow(QWidget *parent): QDialog(parent), m_ui(new Ui::ProfilesWindow)
{
  m_ui->setupUi(this);
  m_ui->versionLabel->setText(QString("KittyIM v%1").arg(Constants::VERSION));

  setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);

  qDebug() << "Creating ProfilesWindow";

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

  QTreeWidgetItem *item = new QTreeWidgetItem();
  item->setText(0, tr("Add new profile"));
  item->setIcon(0, QIcon(":/icons/main.ico"));
  item->setData(0, Qt::UserRole, true);
  m_ui->profilesWidget->addTopLevelItem(item);
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
      Kitty::XmlSettings set(Kitty::Core::inst()->profilesDir() + "/" + current->text(0));
      bool hasPassword = !set.value(KittySDK::Settings::S_PROFILE_PASSWORD).toString().isEmpty();

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
      Kitty::XmlSettings set(Kitty::Core::inst()->profilesDir() + "/" + item->text(0));
      bool hasPassword = !set.value(KittySDK::Settings::S_PROFILE_PASSWORD).toString().isEmpty();

      if(!hasPassword || (hasPassword && set.value(KittySDK::Settings::S_PROFILE_PASSWORD).toString() == QCryptographicHash::hash(m_ui->passwordEdit->text().toLocal8Bit(), QCryptographicHash::Sha1).toHex())) {
        Core::inst()->loadProfile(item->text(0));
        close();
      } else {
        QMessageBox::information(this, tr("Wrong password"), tr("The password you supplied is wrong."));
      }
    } else {
      //new profile
    }
  }
}

void Kitty::ProfilesWindow::on_loginButton_clicked()
{
  on_profilesWidget_itemDoubleClicked(m_ui->profilesWidget->currentItem(), 0);
}
