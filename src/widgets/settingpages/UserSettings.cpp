#include "UserSettings.h"
#include "ui_UserSettings.h"

#include "SDK/constants.h"

using namespace Kitty;
using namespace KittySDK;

Kitty::UserSettings::UserSettings(QWidget *parent): SettingPage(0, parent), m_ui(new Ui::UserSettings)
{
	m_ui->setupUi(this);

	setIcon(Icons::I_USER);
}

Kitty::UserSettings::~UserSettings()
{
	delete m_ui;
}

void Kitty::UserSettings::apply()
{
}

void Kitty::UserSettings::reset()
{
}

void Kitty::UserSettings::retranslate()
{
	m_ui->retranslateUi(this);
}
