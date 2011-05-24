#include "ContactWindow.h"
#include "ui_ContactWindow.h"

#include "ContactManager.h"
#include "AccountManager.h"
#include "SDK/constants.h"
#include "Profile.h"
#include "Core.h"

#include <QtCore/QCryptographicHash>
#include <QtCore/QFile>
#include <QtGui/QFileDialog>

using namespace Kitty;
using namespace KittySDK;

#define qDebug() qDebug() << "[ContactWindow]"
#define qWarning() qWarning() << "[ContactWindow]"

Kitty::ContactWindow::ContactWindow(Contact *cnt, QWidget *parent): QDialog(parent), m_ui(new Ui::ContactWindow), m_contact(cnt)
{
  m_ui->setupUi(this);

  setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
  setAttribute(Qt::WA_DeleteOnClose);

  if(cnt) {
    setWindowTitle(tr("Edit contact"));
  } else {
    setWindowTitle(tr("Add contact"));
  }

  restoreGeometry(Core::inst()->setting(Settings::S_CONTACTWINDOW_GEOMETRY).toByteArray());

  applySettings();
}

Kitty::ContactWindow::~ContactWindow()
{
  Core::inst()->setSetting(Settings::S_CONTACTWINDOW_GEOMETRY, saveGeometry());

  delete m_ui;
}

void Kitty::ContactWindow::applySettings()
{
  Core *core = Core::inst();

  for(int i = 0; i < m_ui->treeWidget->topLevelItemCount(); i++) {
    QTreeWidgetItem *item = m_ui->treeWidget->topLevelItem(i);
    item->setIcon(0, core->icon(Icons::I_BULLET));
  }
}

void Kitty::ContactWindow::on_treeWidget_currentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous)
{
  if(current) {
    m_ui->groupBox->setTitle(current->text(0));
    m_ui->stackedWidget->setCurrentIndex(m_ui->treeWidget->indexOfTopLevelItem(current));
  }
}

void Kitty::ContactWindow::showEvent(QShowEvent *event)
{
  Core *core = Core::inst();

  if(m_contact) {
    m_ui->treeWidget->setCurrentItem(m_ui->treeWidget->topLevelItem(0));
  } else {
    m_ui->treeWidget->setCurrentItem(m_ui->treeWidget->topLevelItem(1));
  }

  //add empty group
  m_ui->groupComboBox->addItem("");
  m_ui->groupComboBox->setCurrentIndex(0);

  //add all the other groups
  QStringList groups = ContactManager::inst()->groups();
  m_ui->groupComboBox->addItems(groups);

  //add all accounts
  foreach(Account *acc, AccountManager::inst()->accounts()) {
    m_ui->accountComboBox->addItem(QString("%1 (%2)").arg(acc->uid()).arg(acc->protocol()->protoInfo()->protoName()));
    m_ui->accountComboBox->setItemIcon(m_ui->accountComboBox->count() - 1, core->icon(acc->protocol()->protoInfo()->protoIcon()));
    m_ui->accountComboBox->setItemData(m_ui->accountComboBox->count() - 1, acc->protocol()->protoInfo()->protoName(), Qt::UserRole + 1);
    m_ui->accountComboBox->setItemData(m_ui->accountComboBox->count() - 1, acc->uid(), Qt::UserRole + 2);
  }

  if(m_contact) {
    //select contact's group
    for(int i = 0; i < m_ui->groupComboBox->count(); i++) {
      if(m_ui->groupComboBox->itemText(i) == m_contact->group()) {
        m_ui->groupComboBox->setCurrentIndex(i);
        break;
      }
    }

    //select contact's account
    for(int i = 0; i < m_ui->accountComboBox->count(); i++) {
      if(m_ui->accountComboBox->itemText(i) == QString("%1 (%2)").arg(m_contact->account()->uid()).arg(m_contact->account()->protocol()->protoInfo()->protoName())) {
        m_ui->accountComboBox->setCurrentIndex(i);
        break;
      }
    }
    m_ui->accountComboBox->setEnabled(false);

    //set
    m_ui->uidEdit->setText(m_contact->uid());
    m_ui->uidEdit->setReadOnly(true);

    //general
    m_ui->displayComboBox->addItem(m_contact->display());
    m_ui->nicknameEdit->setText(m_contact->data(ContactInfos::I_NICKNAME).toString());
    m_ui->firstNameEdit->setText(m_contact->data(ContactInfos::I_FIRSTNAME).toString());
    m_ui->middleNameEdit->setText(m_contact->data(ContactInfos::I_MIDDLENAME).toString());
    m_ui->lastNameEdit->setText(m_contact->data(ContactInfos::I_LASTNAME).toString());
    m_ui->birthdayDateEdit->setDate(m_contact->data(ContactInfos::I_BIRTHDAY).toDate());
    m_ui->sexComboBox->setCurrentIndex(m_contact->data(ContactInfos::I_SEX).toInt());

    QString fileName = Core::inst()->currentProfileDir() + "avatars/" + QCryptographicHash::hash(QString("avatar_" + m_contact->protocol()->protoInfo()->protoName() + "_" + m_contact->uid()).toAscii(), QCryptographicHash::Md5).toHex() + ".png";
    if(QFile::exists(fileName)) {
      m_ui->avatarLabel->setPixmap(QPixmap(fileName));
    }

    //home
    m_ui->homeAddressEdit->setText(m_contact->data(ContactInfos::I_HOME_ADDRESS).toString());
    m_ui->homeCityEdit->setText(m_contact->data(ContactInfos::I_HOME_CITY).toString());
    m_ui->homePostalCodeEdit->setText(m_contact->data(ContactInfos::I_HOME_POSTALCODE).toString());
    m_ui->homeStateEdit->setText(m_contact->data(ContactInfos::I_HOME_STATE).toString());
    m_ui->homeCountryEdit->setText(m_contact->data(ContactInfos::I_HOME_COUNTRY).toString());

    //work
    m_ui->workCompanyEdit->setText(m_contact->data(ContactInfos::I_WORK_COMPANY).toString());
    m_ui->workPositionEdit->setText(m_contact->data(ContactInfos::I_WORK_POSITION).toString());
    m_ui->workAddressEdit->setText(m_contact->data(ContactInfos::I_WORK_ADDRESS).toString());
    m_ui->workCityEdit->setText(m_contact->data(ContactInfos::I_WORK_CITY).toString());
    m_ui->workPostalCodeEdit->setText(m_contact->data(ContactInfos::I_WORK_POSTALCODE).toString());
    m_ui->workStateEdit->setText(m_contact->data(ContactInfos::I_WORK_STATE).toString());
    m_ui->workCountryEdit->setText(m_contact->data(ContactInfos::I_WORK_COUNTRY).toString());
    m_ui->workWebsiteEdit->setText(m_contact->data(ContactInfos::I_WORK_WEBSITE).toString());

    //more
    m_ui->homepageEdit->setText(m_contact->data(ContactInfos::I_WEBSITE).toString());
    m_ui->notesEdit->setPlainText(m_contact->data(ContactInfos::I_NOTES).toString());
  }

  QDialog::showEvent(event);
}

void Kitty::ContactWindow::on_buttonBox_accepted()
{
  if(m_ui->displayComboBox->currentText().isEmpty()) {
    return;
  }

  if(!m_contact) {
    if(m_ui->accountComboBox->count() > 0) {
      if(!m_ui->uidEdit->text().isEmpty()) {
        QString protocol = m_ui->accountComboBox->itemData(m_ui->accountComboBox->currentIndex(), Qt::UserRole + 1).toString();
        QString account = m_ui->accountComboBox->itemData(m_ui->accountComboBox->currentIndex(), Qt::UserRole + 2).toString();

        Account *acc = AccountManager::inst()->account(protocol, account);
        if(acc) {
          m_contact = acc->newContact(m_ui->uidEdit->text());
          m_contact->setGroup(m_ui->groupComboBox->currentText());

          acc->insertContact(m_ui->uidEdit->text(), m_contact);

          ContactManager::inst()->add(m_contact);
        }
      } else {
        return;
      }
    } else {
      return;
    }
  }

  //general
  //m_contact->setGroup(m_ui->groupComboBox->currentText());

  m_contact->setDisplay(m_ui->displayComboBox->currentText());

  m_contact->setData(ContactInfos::I_NICKNAME, m_ui->nicknameEdit->text());
  m_contact->setData(ContactInfos::I_FIRSTNAME, m_ui->firstNameEdit->text());
  m_contact->setData(ContactInfos::I_MIDDLENAME, m_ui->middleNameEdit->text());
  m_contact->setData(ContactInfos::I_LASTNAME, m_ui->lastNameEdit->text());
  m_contact->setData(ContactInfos::I_BIRTHDAY, m_ui->birthdayDateEdit->date());
  m_contact->setData(ContactInfos::I_SEX, m_ui->sexComboBox->currentIndex());

  QString fileName = Core::inst()->currentProfileDir() + "avatars/" + QCryptographicHash::hash(QString("avatar_" + m_contact->protocol()->protoInfo()->protoName() + "_" + m_contact->uid()).toAscii(), QCryptographicHash::Md5).toHex() + ".png";
  if(m_ui->avatarLabel->pixmap()) {
    m_ui->avatarLabel->pixmap()->save(fileName);
  } else {
    QFile::remove(fileName);
  }

  //home
  m_contact->setData(ContactInfos::I_HOME_ADDRESS, m_ui->homeAddressEdit->text());
  m_contact->setData(ContactInfos::I_HOME_CITY, m_ui->homeCityEdit->text());
  m_contact->setData(ContactInfos::I_HOME_POSTALCODE, m_ui->homePostalCodeEdit->text());
  m_contact->setData(ContactInfos::I_HOME_STATE, m_ui->homeStateEdit->text());
  m_contact->setData(ContactInfos::I_HOME_COUNTRY, m_ui->homeCountryEdit->text());

  //work
  m_contact->setData(ContactInfos::I_WORK_COMPANY, m_ui->workCompanyEdit->text());
  m_contact->setData(ContactInfos::I_WORK_POSITION, m_ui->workPositionEdit->text());
  m_contact->setData(ContactInfos::I_WORK_ADDRESS, m_ui->workAddressEdit->text());
  m_contact->setData(ContactInfos::I_WORK_CITY, m_ui->workCityEdit->text());
  m_contact->setData(ContactInfos::I_WORK_POSTALCODE, m_ui->workPostalCodeEdit->text());
  m_contact->setData(ContactInfos::I_WORK_STATE, m_ui->workStateEdit->text());
  m_contact->setData(ContactInfos::I_WORK_COUNTRY, m_ui->workCountryEdit->text());
  m_contact->setData(ContactInfos::I_WORK_WEBSITE, m_ui->workWebsiteEdit->text());

  //more
  m_contact->setData(ContactInfos::I_WEBSITE, m_ui->homepageEdit->text());
  m_contact->setData(ContactInfos::I_NOTES, m_ui->notesEdit->toPlainText());

  close();
}

void Kitty::ContactWindow::on_changeAvatarButton_clicked()
{
  QString fileName = QFileDialog::getOpenFileName(this, tr("Choose avatar file"), "", tr("Image files") + " (*.png *.gif *bmp *jpg *jpeg)");
  if(!fileName.isEmpty()) {
    QPixmap pix(fileName);
    if((pix.width() > 100) || (pix.height() > 100)) {
      pix = pix.scaled(QSize(100, 100), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    }

    m_ui->avatarLabel->setPixmap(pix);
  }
}
