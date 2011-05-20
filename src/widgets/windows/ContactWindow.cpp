#include "ContactWindow.h"
#include "ui_ContactWindow.h"

#include "ContactManager.h"
#include "SDK/constants.h"
#include "Profile.h"
#include "Core.h"

using namespace Kitty;
using namespace KittySDK;

Kitty::ContactWindow::ContactWindow(Contact *cnt, QWidget *parent): QDialog(parent), m_ui(new Ui::ContactWindow), m_contact(cnt)
{
  m_ui->setupUi(this);

  setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);

  if(cnt) {
    setWindowTitle(tr("Edit contact"));
  } else {
    setWindowTitle(tr("Add contact"));
  }

  applySettings();
}

Kitty::ContactWindow::~ContactWindow()
{
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

  m_ui->treeWidget->setCurrentItem(m_ui->treeWidget->topLevelItem(0));

  //add empty group
  m_ui->groupComboBox->addItem("");
  m_ui->groupComboBox->setCurrentIndex(0);

  //add all the other groups
  QStringList groups = ContactManager::inst()->groups();
  m_ui->groupComboBox->addItems(groups);

  if(m_contact) {
    //select contact's group
    for(int i = 0; i < m_ui->groupComboBox->count(); i++) {
      if(m_ui->groupComboBox->itemText(i) == m_contact->group()) {
        m_ui->groupComboBox->setCurrentIndex(i);
        break;
      }
    }

    //general
    m_ui->displayComboBox->addItem(m_contact->display());
    m_ui->nicknameEdit->setText(m_contact->data(ContactInfos::I_NICKNAME).toString());
    m_ui->firstNameEdit->setText(m_contact->data(ContactInfos::I_FIRSTNAME).toString());
    m_ui->middleNameEdit->setText(m_contact->data(ContactInfos::I_MIDDLENAME).toString());
    m_ui->lastNameEdit->setText(m_contact->data(ContactInfos::I_LASTNAME).toString());
    m_ui->birthdayDateEdit->setDate(m_contact->data(ContactInfos::I_BIRTHDAY).toDate());
    m_ui->sexComboBox->setCurrentIndex(m_contact->data(ContactInfos::I_SEX).toInt());

    m_ui->avatarLabel->setPixmap(QPixmap(core->profilesDir() + core->profile()->name() + "/avatars/avatar_" + m_contact->uid() + ".png"));

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
