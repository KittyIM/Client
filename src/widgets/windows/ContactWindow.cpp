#include "ContactWindow.h"
#include "ui_ContactWindow.h"

#include "ContactManager.h"
#include "AccountManager.h"
#include "Profile.h"
#include "Core.h"

#include <SDKConstants.h>

#include <QtCore/QCryptographicHash>
#include <QtCore/QTimer>
#include <QtCore/QFile>
#include <QtGui/QInputDialog>
#include <QtGui/QFileDialog>

#define qDebug() qDebug() << "[ContactWindow]"
#define qWarning() qWarning() << "[ContactWindow]"

namespace Kitty
{

ContactWindow::ContactWindow(Core *core, KittySDK::IContact *cnt, QWidget *parent):
	QDialog(parent, Qt::Window),
	m_ui(new Ui::ContactWindow),
	m_core(core),
	m_contact(cnt)
{
	m_ui->setupUi(this);

	setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
	setAttribute(Qt::WA_DeleteOnClose);

	if(cnt) {
		setWindowTitle(tr("Edit contact") + ": " + cnt->display());
	} else {
		setWindowTitle(tr("Add contact"));
	}

	restoreGeometry(m_core->setting(KittySDK::Settings::S_CONTACTWINDOW_GEOMETRY).toByteArray());

	applySettings();

	QTimer::singleShot(0, this, SLOT(loadData()));
}

ContactWindow::~ContactWindow()
{
	m_core->setSetting(KittySDK::Settings::S_CONTACTWINDOW_GEOMETRY, saveGeometry());

	delete m_ui;
}

void ContactWindow::applySettings()
{
	for(int i = 0; i < m_ui->treeWidget->topLevelItemCount(); ++i) {
		QTreeWidgetItem *item = m_ui->treeWidget->topLevelItem(i);
		item->setIcon(0, m_core->icon(KittySDK::Icons::I_BULLET));
	}

	m_ui->phoneAddButton->setIcon(m_core->icon(KittySDK::Icons::I_ADD));
	m_ui->emailAddButton->setIcon(m_core->icon(KittySDK::Icons::I_ADD));

	m_ui->phoneDeleteButton->setIcon(m_core->icon(KittySDK::Icons::I_DELETE));
	m_ui->emailDeleteButton->setIcon(m_core->icon(KittySDK::Icons::I_DELETE));
}

void ContactWindow::on_treeWidget_currentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous)
{
	if(current) {
		m_ui->groupBox->setTitle(current->text(0));
		m_ui->stackedWidget->setCurrentIndex(m_ui->treeWidget->indexOfTopLevelItem(current));
	}
}

void ContactWindow::on_buttonBox_accepted()
{
	if(m_ui->displayComboBox->currentText().isEmpty()) {
		return;
	}

	if(!m_contact) {
		if(m_ui->accountComboBox->count() > 0) {
			if(!m_ui->uidEdit->text().isEmpty()) {
				QString protocol = m_ui->accountComboBox->itemData(m_ui->accountComboBox->currentIndex(), Qt::UserRole + 1).toString();
				QString account = m_ui->accountComboBox->itemData(m_ui->accountComboBox->currentIndex(), Qt::UserRole + 2).toString();

				KittySDK::IAccount *acc = m_core->accountManager()->account(protocol, account);
				if(acc) {
					m_contact = acc->newContact(m_ui->uidEdit->text());
					m_contact->setGroup(m_ui->groupComboBox->currentText());

					acc->insertContact(m_ui->uidEdit->text(), m_contact);

					m_core->contactManager()->add(m_contact);
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

	m_contact->setData(KittySDK::ContactInfos::I_NICKNAME, m_ui->nicknameEdit->text());
	m_contact->setData(KittySDK::ContactInfos::I_FIRSTNAME, m_ui->firstNameEdit->text());
	m_contact->setData(KittySDK::ContactInfos::I_MIDDLENAME, m_ui->middleNameEdit->text());
	m_contact->setData(KittySDK::ContactInfos::I_LASTNAME, m_ui->lastNameEdit->text());

	if(m_ui->birthdayDateEdit->date() != QDate(1900, 1, 1)) {
		m_contact->setData(KittySDK::ContactInfos::I_BIRTHDAY, m_ui->birthdayDateEdit->date());
	}

	m_contact->setData(KittySDK::ContactInfos::I_SEX, m_ui->sexComboBox->currentIndex());

	QString fileName = m_core->avatarPath(m_contact);
	if(m_ui->avatarLabel->pixmap()) {
		m_ui->avatarLabel->pixmap()->save(fileName);
	} else {
		QFile::remove(fileName);
	}

	//home
	m_contact->setData(KittySDK::ContactInfos::I_HOME_ADDRESS, m_ui->homeAddressEdit->text());
	m_contact->setData(KittySDK::ContactInfos::I_HOME_CITY, m_ui->homeCityEdit->text());
	m_contact->setData(KittySDK::ContactInfos::I_HOME_POSTALCODE, m_ui->homePostalCodeEdit->text());
	m_contact->setData(KittySDK::ContactInfos::I_HOME_STATE, m_ui->homeStateEdit->text());
	m_contact->setData(KittySDK::ContactInfos::I_HOME_COUNTRY, m_ui->homeCountryEdit->text());

	//contact
	QStringList emails;
	foreach(QListWidgetItem *item, m_ui->emailListWidget->findItems("*", Qt::MatchWildcard)) {
		if(!item->text().isEmpty()) {
			emails.append(item->text());
		}
	}
	emails.removeDuplicates();
	m_contact->setData(KittySDK::ContactInfos::I_EMAILS, emails.join(","));

	QStringList phones;
	foreach(QListWidgetItem *item, m_ui->phoneListWidget->findItems("*", Qt::MatchWildcard)) {
		if(!item->text().isEmpty()) {
			phones.append(item->text());
		}
	}
	phones.removeDuplicates();
	m_contact->setData(KittySDK::ContactInfos::I_PHONES, phones.join(","));

	//work
	m_contact->setData(KittySDK::ContactInfos::I_WORK_COMPANY, m_ui->workCompanyEdit->text());
	m_contact->setData(KittySDK::ContactInfos::I_WORK_POSITION, m_ui->workPositionEdit->text());
	m_contact->setData(KittySDK::ContactInfos::I_WORK_ADDRESS, m_ui->workAddressEdit->text());
	m_contact->setData(KittySDK::ContactInfos::I_WORK_CITY, m_ui->workCityEdit->text());
	m_contact->setData(KittySDK::ContactInfos::I_WORK_POSTALCODE, m_ui->workPostalCodeEdit->text());
	m_contact->setData(KittySDK::ContactInfos::I_WORK_STATE, m_ui->workStateEdit->text());
	m_contact->setData(KittySDK::ContactInfos::I_WORK_COUNTRY, m_ui->workCountryEdit->text());
	m_contact->setData(KittySDK::ContactInfos::I_WORK_WEBSITE, m_ui->workWebsiteEdit->text());

	//more
	m_contact->setData(KittySDK::ContactInfos::I_HOMEPAGE, m_ui->homepageEdit->text());
	m_contact->setData(KittySDK::ContactInfos::I_NOTES, m_ui->notesEdit->toPlainText());

	accept();
}

void ContactWindow::on_changeAvatarButton_clicked()
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

void ContactWindow::closeEvent(QCloseEvent *event)
{
	emit rejected();

	QWidget::closeEvent(event);
}

void ContactWindow::changeEvent(QEvent *event)
{
	if(event->type() == QEvent::LanguageChange) {
		m_ui->retranslateUi(this);
		updateSummary();
	}

	QDialog::changeEvent(event);
}

void ContactWindow::on_emailAddButton_clicked()
{
	QString email = QInputDialog::getText(this, tr("Email"), tr("Please specify an email address:"));
	if(!email.isEmpty()) {
		if(m_ui->emailListWidget->findItems(email, Qt::MatchExactly).count() == 0) {
			QListWidgetItem *item = new QListWidgetItem(email, m_ui->emailListWidget);
			item->setFlags(item->flags() | Qt::ItemIsEditable);
		}
	}
}

void ContactWindow::on_emailDeleteButton_clicked()
{
	if(m_ui->emailListWidget->selectedItems().count() > 0) {
		delete m_ui->emailListWidget->currentItem();
	}
}

void ContactWindow::on_emailListWidget_currentItemChanged(QListWidgetItem *current, QListWidgetItem *previous)
{
	m_ui->emailDeleteButton->setEnabled(current != 0);
}

void ContactWindow::on_phoneAddButton_clicked()
{
	QString phone = QInputDialog::getText(this, tr("Phone"), tr("Please specify a phone number:"));
	if(!phone.isEmpty()) {
		if(m_ui->phoneListWidget->findItems(phone, Qt::MatchExactly).count() == 0) {
			QListWidgetItem *item = new QListWidgetItem(phone, m_ui->phoneListWidget);
			item->setFlags(item->flags() | Qt::ItemIsEditable);
		}
	}
}

void ContactWindow::on_phoneDeleteButton_clicked()
{
	if(m_ui->phoneListWidget->selectedItems().count() > 0) {
		delete m_ui->phoneListWidget->currentItem();
	}
}

void ContactWindow::on_phoneListWidget_currentItemChanged(QListWidgetItem *current, QListWidgetItem *previous)
{
	m_ui->phoneDeleteButton->setEnabled(current != 0);
}


void ContactWindow::loadData()
{
	if(!m_contact) {
		//remove summary when adding contact
		m_ui->stackedWidget->removeWidget(m_ui->stackedWidget->widget(0));
		delete m_ui->treeWidget->topLevelItem(0);
	} else {
		updateSummary();
	}

	m_ui->treeWidget->setCurrentItem(m_ui->treeWidget->topLevelItem(0));

	//add empty group
	m_ui->groupComboBox->addItem("");
	m_ui->groupComboBox->setCurrentIndex(0);

	//add all the other groups
	QStringList groups = m_core->contactManager()->groups();
	m_ui->groupComboBox->addItems(groups);

	//add all accounts
	foreach(KittySDK::IAccount *acc, m_core->accountManager()->accounts()) {
		m_ui->accountComboBox->addItem(QString("%1 (%2)").arg(acc->uid()).arg(acc->protocol()->protoInfo()->protoName()));
		m_ui->accountComboBox->setItemIcon(m_ui->accountComboBox->count() - 1, m_core->icon(acc->protocol()->protoInfo()->protoIcon()));
		m_ui->accountComboBox->setItemData(m_ui->accountComboBox->count() - 1, acc->protocol()->protoInfo()->protoName(), Qt::UserRole + 1);
		m_ui->accountComboBox->setItemData(m_ui->accountComboBox->count() - 1, acc->uid(), Qt::UserRole + 2);
	}

	if(m_contact) {
		//select contact's group
		for(int i = 0; i < m_ui->groupComboBox->count(); ++i) {
			if(m_ui->groupComboBox->itemText(i) == m_contact->group()) {
				m_ui->groupComboBox->setCurrentIndex(i);
				break;
			}
		}

		//select contact's account
		for(int i = 0; i < m_ui->accountComboBox->count(); ++i) {
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
		m_ui->nicknameEdit->setText(m_contact->data(KittySDK::ContactInfos::I_NICKNAME).toString());
		m_ui->firstNameEdit->setText(m_contact->data(KittySDK::ContactInfos::I_FIRSTNAME).toString());
		m_ui->middleNameEdit->setText(m_contact->data(KittySDK::ContactInfos::I_MIDDLENAME).toString());
		m_ui->lastNameEdit->setText(m_contact->data(KittySDK::ContactInfos::I_LASTNAME).toString());
		m_ui->birthdayDateEdit->setDate(m_contact->data(KittySDK::ContactInfos::I_BIRTHDAY).toDate());
		m_ui->sexComboBox->setCurrentIndex(m_contact->data(KittySDK::ContactInfos::I_SEX).toInt());

		QString fileName = m_core->avatarPath(m_contact);
		if(QFile::exists(fileName)) {
			m_ui->avatarLabel->setPixmap(QPixmap(fileName));
		}

		//contact
		QStringList emails = m_contact->data(KittySDK::ContactInfos::I_EMAILS).toString().split(",");
		foreach(QString email, emails) {
			if(!email.isEmpty()) {
				QListWidgetItem *item = new QListWidgetItem(email, m_ui->emailListWidget);
				item->setFlags(item->flags() | Qt::ItemIsEditable);
			}
		}

		QStringList phones = m_contact->data(KittySDK::ContactInfos::I_PHONES).toString().split(",");
		foreach(QString phone, phones) {
			if(!phone.isEmpty()) {
				QListWidgetItem *item = new QListWidgetItem(phone, m_ui->phoneListWidget);
				item->setFlags(item->flags() | Qt::ItemIsEditable);
			}
		}

		//home
		m_ui->homeAddressEdit->setText(m_contact->data(KittySDK::ContactInfos::I_HOME_ADDRESS).toString());
		m_ui->homeCityEdit->setText(m_contact->data(KittySDK::ContactInfos::I_HOME_CITY).toString());
		m_ui->homePostalCodeEdit->setText(m_contact->data(KittySDK::ContactInfos::I_HOME_POSTALCODE).toString());
		m_ui->homeStateEdit->setText(m_contact->data(KittySDK::ContactInfos::I_HOME_STATE).toString());
		m_ui->homeCountryEdit->setText(m_contact->data(KittySDK::ContactInfos::I_HOME_COUNTRY).toString());

		//work
		m_ui->workCompanyEdit->setText(m_contact->data(KittySDK::ContactInfos::I_WORK_COMPANY).toString());
		m_ui->workPositionEdit->setText(m_contact->data(KittySDK::ContactInfos::I_WORK_POSITION).toString());
		m_ui->workAddressEdit->setText(m_contact->data(KittySDK::ContactInfos::I_WORK_ADDRESS).toString());
		m_ui->workCityEdit->setText(m_contact->data(KittySDK::ContactInfos::I_WORK_CITY).toString());
		m_ui->workPostalCodeEdit->setText(m_contact->data(KittySDK::ContactInfos::I_WORK_POSTALCODE).toString());
		m_ui->workStateEdit->setText(m_contact->data(KittySDK::ContactInfos::I_WORK_STATE).toString());
		m_ui->workCountryEdit->setText(m_contact->data(KittySDK::ContactInfos::I_WORK_COUNTRY).toString());
		m_ui->workWebsiteEdit->setText(m_contact->data(KittySDK::ContactInfos::I_WORK_WEBSITE).toString());

		//more
		m_ui->homepageEdit->setText(m_contact->data(KittySDK::ContactInfos::I_HOMEPAGE).toString());
		m_ui->notesEdit->setPlainText(m_contact->data(KittySDK::ContactInfos::I_NOTES).toString());
	}
}


void ContactWindow::finishEditing()
{
	QString current = m_ui->displayComboBox->currentText();
	m_ui->displayComboBox->clear();

	if(!current.isEmpty()) {
		m_ui->displayComboBox->addItem(current);
	}

	QString text = m_ui->nicknameEdit->text();
	if(!text.isEmpty() && (text != current)) {
		m_ui->displayComboBox->addItem(text);
	}

	text = m_ui->firstNameEdit->text();
	if(!text.isEmpty() && (text != current)) {
		m_ui->displayComboBox->addItem(text);
	}

	text = m_ui->lastNameEdit->text();
	if(!text.isEmpty() && (text != current)) {
		m_ui->displayComboBox->addItem(text);
	}

	if(!m_ui->firstNameEdit->text().isEmpty() && !m_ui->lastNameEdit->text().isEmpty()) {
		text = QString("%1 %2").arg(m_ui->firstNameEdit->text()).arg(m_ui->lastNameEdit->text());
		if(!text.isEmpty() && (text != current)) {
			m_ui->displayComboBox->addItem(text);
		}

		if(!m_ui->nicknameEdit->text().isEmpty()) {
			text = QString("%1 \"%2\" %3").arg(m_ui->firstNameEdit->text()).arg(m_ui->nicknameEdit->text()).arg(m_ui->lastNameEdit->text());
			if(text != current) {
				m_ui->displayComboBox->addItem(text);
			}
		}

		text = QString("%2 %1").arg(m_ui->firstNameEdit->text()).arg(m_ui->lastNameEdit->text());
		if(text != current) {
			m_ui->displayComboBox->addItem(text);
		}
	}
}

void ContactWindow::updateSummary()
{
	QString summary;
	if(m_contact->data(KittySDK::ContactInfos::I_FIRSTNAME).toBool() || m_contact->data(KittySDK::ContactInfos::I_MIDDLENAME).toBool() || m_contact->data(KittySDK::ContactInfos::I_NICKNAME).toBool() || m_contact->data(KittySDK::ContactInfos::I_LASTNAME).toBool()) {
		QString nickname = m_contact->data(KittySDK::ContactInfos::I_NICKNAME).toString();
		if(!nickname.isEmpty()) {
			nickname.prepend("\"");
			nickname.append("\" ");
		}

		summary += QString("<center><b>%1%2%3%4</b></center>").arg(Qt::escape(m_contact->data(KittySDK::ContactInfos::I_FIRSTNAME).toString()).append(" ")).arg(Qt::escape(m_contact->data(KittySDK::ContactInfos::I_MIDDLENAME).toString()).append(" ")).arg(Qt::escape(nickname)).arg(Qt::escape(m_contact->data(KittySDK::ContactInfos::I_LASTNAME).toString()).append(" "));
	} else {
		summary += QString("<center><b>%1</b></center>").arg(Qt::escape(m_contact->display()));
	}

	QDate birthday = m_contact->data(KittySDK::ContactInfos::I_BIRTHDAY).toDate();
	int sex = m_contact->data(KittySDK::ContactInfos::I_SEX).toInt();

	if(sex || (birthday != QDate(1900, 1, 1))) {
		summary += "<center>";
	}

	if(sex == 1) {
		summary += tr("female");
	} else if(sex == 2) {
		summary += tr("male");
	}

	if(birthday != QDate(1900, 1, 1)) {
		summary += " <b>";

		int years = QDate::currentDate().year() - birthday.year();
		if(QDate::currentDate() < birthday.addYears(years)) {
			years--;
		}

		summary += QString(tr("%1 years old")).arg(years);
		summary += "</b>";
	}

	if(sex || (birthday != QDate(1900, 1, 1))) {
		summary += "</center>";
		summary += "<br>";
	}

	summary += "<b>" + tr("Account") + ":</b> " + Qt::escape(m_contact->protocol()->protoInfo()->protoName()) + " " + m_contact->account()->uid();
	summary += "<br>";

	summary += "<b>" + tr("UID") + ":</b> " + Qt::escape(m_contact->uid());
	summary += "<br>";

	QString description = m_contact->description();
	if(!description.isEmpty()) {
		description.prepend(" \"");
		description.append("\"");
	}
	summary += "<b>" + tr("Status") + ":</b> " + m_core->statusToString(m_contact->status()) + Qt::escape(description);
	summary += "<br>";

	if(!m_contact->data(KittySDK::ContactInfos::I_HOMEPAGE).toString().isEmpty()) {
		summary += "<b>" + tr("Homepage") + ":</b> " + QString("<a href=\"%1\">%1</a>").arg(Qt::escape(m_contact->data(KittySDK::ContactInfos::I_HOMEPAGE).toString()));
		summary += "<br>";
	}

	if(!m_contact->data(KittySDK::ContactInfos::I_EMAILS).toString().isEmpty()) {
		QStringList list = m_contact->data(KittySDK::ContactInfos::I_EMAILS).toString().split(",");
		QString emails;
		foreach(QString email, list) {
			if(!email.isEmpty()) {
				emails += QString("<a href=\"mailto:%1\">%1</a>, ").arg(Qt::escape(email));
			}
		}

		emails.chop(2);

		summary += "<b>" + tr("Email(s)", "", list.count()) + ":</b> " + emails;
		summary += "<br>";
	}

	if(!m_contact->data(KittySDK::ContactInfos::I_PHONES).toString().isEmpty()) {
		summary += "<b>" + tr("Phone(s)", "", m_contact->data(KittySDK::ContactInfos::I_PHONES).toString().split(",").count()) + ":</b> " + Qt::escape(m_contact->data(KittySDK::ContactInfos::I_PHONES).toString());
		summary += "<br>";
	}

	QString address = m_contact->data(KittySDK::ContactInfos::I_HOME_ADDRESS).toString();
	QString city = m_contact->data(KittySDK::ContactInfos::I_HOME_CITY).toString();
	QString country = m_contact->data(KittySDK::ContactInfos::I_HOME_COUNTRY).toString();
	QString code = m_contact->data(KittySDK::ContactInfos::I_HOME_POSTALCODE).toString();
	QString state = m_contact->data(KittySDK::ContactInfos::I_HOME_STATE).toString();

	if(!state.isEmpty() || !code.isEmpty() || !country.isEmpty() || !city.isEmpty() || !address.isEmpty()) {
		summary += "<center><b><u>" + tr("Home") + "</u></b><br>";

		if(!address.isEmpty()) {
			summary += Qt::escape(address) + "<br>";
		}

		if(!code.isEmpty() || !city.isEmpty()) {
			summary += Qt::escape(code) + " " + Qt::escape(city) + "<br>";
		}

		if(!state.isEmpty()) {
			summary += Qt::escape(state) + "<br>";
		}

		if(!country.isEmpty()) {
			summary += Qt::escape(country) + "<br>";
		}
	}

	address = m_contact->data(KittySDK::ContactInfos::I_WORK_ADDRESS).toString();
	city = m_contact->data(KittySDK::ContactInfos::I_WORK_CITY).toString();
	country = m_contact->data(KittySDK::ContactInfos::I_WORK_COUNTRY).toString();
	code = m_contact->data(KittySDK::ContactInfos::I_WORK_POSTALCODE).toString();
	state = m_contact->data(KittySDK::ContactInfos::I_WORK_STATE).toString();
	QString company = m_contact->data(KittySDK::ContactInfos::I_WORK_COMPANY).toString();
	QString position = m_contact->data(KittySDK::ContactInfos::I_WORK_POSITION).toString();
	QString website = m_contact->data(KittySDK::ContactInfos::I_WORK_WEBSITE).toString();

	if(!state.isEmpty() || !code.isEmpty() || !country.isEmpty() || !city.isEmpty() || !address.isEmpty() || !company.isEmpty() || !position.isEmpty() || !website.isEmpty()) {
		summary += "<center><b><u>" + tr("Work") + "</u></b><br>";

		if(!company.isEmpty() || !position.isEmpty()) {
			summary += Qt::escape(position);

			if(!company.isEmpty() && !position.isEmpty()) {
				summary += " " + tr("in") + " ";
			}

			summary += Qt::escape(company) + "<br>";
		}

		if(!address.isEmpty()) {
			summary += Qt::escape(address) + "<br>";
		}

		if(!code.isEmpty() || !city.isEmpty()) {
			summary += Qt::escape(code) + " " + Qt::escape(city) + "<br>";
		}

		if(!state.isEmpty()) {
			summary += Qt::escape(state) + "<br>";
		}

		if(!country.isEmpty()) {
			summary += Qt::escape(country) + "<br>";
		}

		if(!website.isEmpty()) {
			summary += QString("<a href=\"%1\">%1</a>").arg(Qt::escape(website));
		}
	}

	m_ui->summaryTextBrowser->setHtml(summary);
}

}
