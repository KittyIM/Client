#include "RosterHeader.h"
#include "ui_RosterHeader.h"

#include "AccountManager.h"
#include "Profile.h"
#include "Core.h"

#include <QtCore/QDebug>
#include <QtGui/QStylePainter>

using namespace Kitty;
using namespace KittySDK;

Kitty::RosterHeader::RosterHeader(QWidget *parent): QWidget(parent), m_ui(new Ui::RosterHeader)
{
  m_ui->setupUi(this);
  connect(m_ui->statusTextEdit, SIGNAL(returnPressed(QString)), this, SIGNAL(descriptionChanged(QString)));
  connect(m_ui->statusChangeButton, SIGNAL(statusChanged(KittySDK::Protocol::Status)), this, SIGNAL(statusChanged(KittySDK::Protocol::Status)));
  connect(AccountManager::inst(), SIGNAL(accountStatusChanged(KittySDK::Account*,KittySDK::Protocol::Status,QString)), this, SLOT(updateWidgets()));
}

Kitty::RosterHeader::~RosterHeader()
{
  delete m_ui;
}

void Kitty::RosterHeader::applySettings()
{
  Core *core = Core::inst();

  m_ui->profileLabel->setText(core->profile()->name());
  m_ui->avatarLabel->setPixmap(QPixmap(core->profilesDir() + core->profile()->name() + "/avatar.png"));
}

void Kitty::RosterHeader::updateWidgets()
{
  Protocol::Status status = Protocol::Offline;
  foreach(Account *acc, AccountManager::inst()->accounts()) {
    status = qMin(status, acc->status());
  }

  m_ui->statusChangeButton->setStatus(status);
}

void Kitty::RosterHeader::paintEvent(QPaintEvent *event)
{
  QStyleOption opt;
  opt.init(this);

  QStylePainter p(this);
  p.drawPrimitive(QStyle::PE_Widget, opt);
}

