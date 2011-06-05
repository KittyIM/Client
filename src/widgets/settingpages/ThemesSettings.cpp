#include "ThemesSettings.h"
#include "ui_ThemesSettings.h"

#include "PluginCoreImpl.h"
#include "SDK/constants.h"
#include "SDK/Protocol.h"
#include "SDK/Message.h"
#include "SDK/Contact.h"
#include "SDK/Account.h"
#include "ChatTheme.h"
#include "Profile.h"
#include "Core.h"

#include <QtCore/QFileInfoList>
#include <QtCore/QDateTime>
#include <QtCore/QDebug>
#include <QtCore/QDir>
#include <QtWebKit/QWebElement>
#include <QtWebKit/QWebFrame>

using namespace Kitty;
using namespace KittySDK;

class KittyAccount: public Account
{
  public:
    KittyAccount(const QString &uid, Protocol *parent): Account(uid, parent)
    {
      setMe(new Contact("me@example.com", this));
      me()->setDisplay(protocol()->core()->profileName());
    }

    ~KittyAccount()
    {
      delete m_me;
    }

    Contact *newContact(const QString &uid){ return new Contact(uid, this); }
};

class KittyProto: public Protocol
{
  public:
    KittyProto(PluginCore *core): Protocol(core)
    {
      m_info = new ProtocolInfo("", "", "", "", "", "KittyIM");
    }

    ~KittyProto()
    {
      delete m_info;
    }

    Account *newAccount(const QString &uid) { return new KittyAccount(uid, this); }
    QWidget *editWindow(Account *) { return 0; }
};

Kitty::ThemesSettings::ThemesSettings(QWidget *parent): SettingPage(0, parent), m_ui(new Ui::ThemesSettings)
{
  m_ui->setupUi(this);

  setIcon(Icons::I_BULLET);

  connect(m_ui->chatThemeComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(updateVariantList()));
}

Kitty::ThemesSettings::~ThemesSettings()
{
  delete m_ui;
}

void Kitty::ThemesSettings::apply()
{
  Core *core = Core::inst();
  core->setSetting(Settings::S_ROSTER_THEME, m_ui->rosterThemeComboBox->itemData(m_ui->rosterThemeComboBox->currentIndex()));
  core->setSetting(Settings::S_CHAT_THEME, m_ui->chatThemeComboBox->itemData(m_ui->chatThemeComboBox->currentIndex()));
  core->setSetting(Settings::S_CHAT_THEME_VARIANT, m_ui->chatThemeVariantComboBox->itemData(m_ui->chatThemeVariantComboBox->currentIndex()));
}

void Kitty::ThemesSettings::reset()
{
  Core *core = Core::inst();

  disconnect(m_ui->chatThemeComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(updateChatPreview()));
  disconnect(m_ui->chatThemeVariantComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(updateChatPreview()));

  m_ui->chatThemeComboBox->clear();
  m_ui->chatThemeComboBox->addItem(tr("Default"), QString());

  QDir chatThemeDir(qApp->applicationDirPath() + "/themes/chat/");
  QFileInfoList chatThemes = chatThemeDir.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot);
  foreach(QFileInfo dir, chatThemes) {
    m_ui->chatThemeComboBox->addItem(dir.fileName(), dir.fileName());

    if(dir.fileName() == core->setting(Settings::S_CHAT_THEME, QString()).toString()) {
      m_ui->chatThemeComboBox->setCurrentIndex(m_ui->chatThemeComboBox->count() - 1);
    }
  }

  updateVariantList();

  m_ui->rosterThemeComboBox->clear();
  m_ui->rosterThemeComboBox->addItem(tr("Default"), QString());

  QDir rosterThemeDir(qApp->applicationDirPath() + "/themes/roster/");
  QFileInfoList rosterThemes = rosterThemeDir.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot);
  foreach(QFileInfo dir, rosterThemes) {
    m_ui->rosterThemeComboBox->addItem(dir.fileName(), dir.fileName());

    if(dir.fileName() == core->setting(Settings::S_ROSTER_THEME, QString()).toString()) {
      m_ui->rosterThemeComboBox->setCurrentIndex(m_ui->rosterThemeComboBox->count() - 1);
    }
  }
}

void Kitty::ThemesSettings::updateChatPreview()
{
  m_ui->chatThemeWebView->clearTo(true, m_ui->chatThemeComboBox->itemData(m_ui->chatThemeComboBox->currentIndex()).toString(), m_ui->chatThemeVariantComboBox->itemData(m_ui->chatThemeVariantComboBox->currentIndex()).toString());

  ChatTheme theme(m_ui->chatThemeComboBox->currentText());

  PluginCoreImpl core;
  KittyProto proto(&core);
  KittyAccount *acc = dynamic_cast<KittyAccount*>(proto.newAccount("me@example.com"));
  Contact *kittyBot = acc->newContact("kittybot@kittyim.pl");
  kittyBot->setDisplay("KittyBot");

  Message outgoing(acc->me(), kittyBot);
  outgoing.setBody("Hello KittyBot!");
  m_ui->chatThemeWebView->appendMessage(outgoing, &theme);

  Message incoming(kittyBot, acc->me());
  incoming.setDirection(Message::Incoming);
  incoming.setBody("Oh hi, I didn't notice you there ;)");
  m_ui->chatThemeWebView->appendMessage(incoming, &theme);

  incoming.setTimeStamp(QDateTime::currentDateTime());
  incoming.setBody("Could you please change your status to online?");
  m_ui->chatThemeWebView->appendMessage(incoming, &theme);

  Message status(acc->me(), acc->me());
  status.setBody(QString("%1 changed status to online").arg(acc->me()->display()));
  status.setDirection(Message::System);
  m_ui->chatThemeWebView->appendMessage(status, &theme);

  outgoing.setTimeStamp(QDateTime::currentDateTime());
  outgoing.setBody("How's that?");
  m_ui->chatThemeWebView->appendMessage(outgoing, &theme);

  incoming.setTimeStamp(QDateTime::currentDateTime());
  incoming.setBody("Awesome, thanks!");
  m_ui->chatThemeWebView->appendMessage(incoming, &theme);

  delete kittyBot;
  delete acc;
}

void Kitty::ThemesSettings::updateVariantList()
{
  Core *core = Core::inst();

  disconnect(m_ui->chatThemeComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(updateChatPreview()));
  disconnect(m_ui->chatThemeVariantComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(updateChatPreview()));

  m_ui->chatThemeVariantComboBox->clear();

  QString path = ":/chat";
  if(!m_ui->chatThemeComboBox->itemData(m_ui->chatThemeComboBox->currentIndex()).toString().isEmpty()) {
    path = qApp->applicationDirPath() + "/themes/chat/" + m_ui->chatThemeComboBox->currentText();
  }

  QDir variantDir(path + "/Variants");
  QFileInfoList variants = variantDir.entryInfoList(QStringList() << "*.css");
  foreach(QFileInfo var, variants) {
    m_ui->chatThemeVariantComboBox->addItem(var.completeBaseName(), var.fileName());

    if(var.fileName() == core->setting(Settings::S_CHAT_THEME_VARIANT, QString()).toString()) {
      m_ui->chatThemeVariantComboBox->setCurrentIndex(m_ui->chatThemeVariantComboBox->count() - 1);
    }
  }

  connect(m_ui->chatThemeComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(updateChatPreview()));
  connect(m_ui->chatThemeVariantComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(updateChatPreview()));

  updateChatPreview();
}
