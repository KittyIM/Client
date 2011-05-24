#include "ChatTab.h"
#include "ui_ChatTab.h"

#include "widgets/windows/ChatWindow.h"
#include "widgets/ChatColorPicker.h"
#include "PluginManager.h"
#include "SDK/constants.h"
#include "SDK/Protocol.h"
#include "SDK/Contact.h"
#include "SDK/Message.h"
#include "IconManager.h"
#include "ChatTheme.h"
#include "SDK/Chat.h"
#include "Profile.h"
#include "Core.h"

#include <QtCore/QFile>
#include <QtGui/QToolBar>
#include <QtWebKit/QWebElement>
#include <QtWebKit/QWebFrame>
#include <QtWebKit/QWebPage>

#define qDebug() qDebug() << "[ChatTab]"
#define qWarning() qWarning() << "[ChatTab]"

using namespace Kitty;
using namespace KittySDK;

Kitty::ChatTab::ChatTab(Chat *chat, QWidget *parent): QWidget(parent), m_ui(new Ui::ChatTab), m_chat(chat)
{
  m_ui->setupUi(this);

  connect(m_ui->textEdit, SIGNAL(returnPressed()), this, SLOT(sendMessage()));
  connect(m_ui->textEdit, SIGNAL(cursorPositionChanged()), this, SLOT(updateButtons()));
  connect(m_ui->webView, SIGNAL(keyPressed()), m_ui->textEdit, SLOT(setFocus()));
  connect(chat->contacts().first(), SIGNAL(statusChanged(KittySDK::Protocol::Status,QString)), this, SLOT(changeStatus(KittySDK::Protocol::Status, QString)));
  connect(chat->contacts().first(), SIGNAL(dataChanged()), this, SIGNAL(tabChanged()));

  m_toolBar = new QToolBar(this);
  m_toolBar->setIconSize(QSize(16, 16));
  m_toolBar->setStyleSheet("QToolBar { border: 0; }");
  m_ui->verticalLayout->addWidget(m_toolBar);

  m_colorPicker = new ChatColorPicker(this);
  connect(m_colorPicker, SIGNAL(colorSelected(QColor)), m_ui->textEdit, SLOT(colorText(QColor)));

  Protocol *proto = chat->account()->protocol();

  if(proto->abilities().testFlag(Protocol::TextBold)) {
    m_boldAction = m_toolBar->addAction(tr("Bold"), m_ui->textEdit, SLOT(boldText()));
    m_boldAction->setCheckable(true);
    m_boldAction->setProperty("icon_id", Icons::I_BOLD);
  } else {
    m_boldAction = 0;
  }

  if(proto->abilities().testFlag(Protocol::TextItalics)) {
    m_italicAction = m_toolBar->addAction(tr("Italic"), m_ui->textEdit, SLOT(italicText()));
    m_italicAction->setCheckable(true);
    m_italicAction->setProperty("icon_id", Icons::I_ITALIC);
  } else {
    m_italicAction = 0;
  }

  if(proto->abilities().testFlag(Protocol::TextUnderline)) {
    m_underlineAction = m_toolBar->addAction(tr("Underline"), m_ui->textEdit, SLOT(underlineText()));
    m_underlineAction->setCheckable(true);
    m_underlineAction->setProperty("icon_id", Icons::I_UNDERLINE);
  } else {
    m_underlineAction = 0;
  }

  if(proto->abilities().testFlag(Protocol::TextStrikethrough)) {
    m_strikethroughAction = m_toolBar->addAction(tr("Strikethrough"));
    m_strikethroughAction->setCheckable(true);
    m_strikethroughAction->setProperty("icon_id", Icons::I_STRIKETHROUGH);
  } else {
    m_strikethroughAction = 0;
  }

  if(m_toolBar->actions().count() > 0) {
    m_toolBar->addSeparator();
  }

  if(proto->abilities().testFlag(Protocol::TextColor)) {
    QAction *colorAction = m_toolBar->addAction(tr("Color"), this, SLOT(showColorPicker()));
    colorAction->setProperty("icon_id", Icons::I_COLOR);
  }

  QAction *smiliesAction = m_toolBar->addAction(tr("Smilies"));
  smiliesAction->setProperty("icon_id", Icons::I_SMILEY);

  m_toolBar->addSeparator();

  int c = m_toolBar->actions().count();

  if(proto->abilities().testFlag(Protocol::SendImages)) {
    QAction *imageAction = m_toolBar->addAction(tr("Send image"));
    imageAction->setProperty("icon_id", Icons::I_IMAGE);
  }

  if(proto->abilities().testFlag(Protocol::SendFiles)) {
    QAction *fileAction = m_toolBar->addAction(tr("Send file"));
    fileAction->setProperty("icon_id", Icons::I_FILE);
  }

  if(m_toolBar->actions().count() > c) {
    m_toolBar->addSeparator();
  }

  QAction *profileAction = m_toolBar->addAction(tr("Profile"));
  profileAction->setProperty("icon_id", Icons::I_PROFILE);

  QAction *historyAction = m_toolBar->addAction(tr("History"));
  historyAction->setProperty("icon_id", Icons::I_HISTORY);

  updateIcons();

  m_ui->webView->clear();
}

Kitty::ChatTab::~ChatTab()
{
  delete m_ui;
}

void Kitty::ChatTab::updateIcons()
{
  Core *core = Core::inst();

  foreach(QAction *action, m_toolBar->actions()) {
    if(action->property("icon_id").isValid()) {
      action->setIcon(core->icon(action->property("icon_id").toString()));
    }
  }
}

void Kitty::ChatTab::setEditFocus()
{
  m_ui->textEdit->setFocus();
}

void Kitty::ChatTab::applySettings()
{
  m_ui->textEdit->clearHistory();
}

void Kitty::ChatTab::appendMessage(KittySDK::Message &msg)
{
  m_ui->webView->appendMessage(msg);
}

void Kitty::ChatTab::sendMessage()
{
  if(!m_ui->textEdit->toPlainText().isEmpty()) {
    qDebug() << m_ui->textEdit->toHtml().replace("<", "&lt;");

    Message msg(m_chat->me(), m_chat->contacts());
    msg.setBody(m_ui->textEdit->toPlainText());

    QList<Plugin*> plugins = PluginManager::inst()->plugins();
    foreach(Plugin *plugin, plugins) {
      if(plugin->isLoaded()) {
        plugin->plugin()->processMessage(msg);
      }
    }

    m_ui->webView->appendMessage(msg);

    if(Core::inst()->setting(Settings::S_CHATWINDOW_SENTHISTORY, true).toBool()) {
      m_ui->textEdit->addHistory(msg.body());
    }

    m_chat->account()->sendMessage(msg);
    m_ui->textEdit->clear();
  }
}

void Kitty::ChatTab::changeStatus(KittySDK::Protocol::Status status, QString description)
{
  emit tabChanged();
}

void Kitty::ChatTab::changeEvent(QEvent *event)
{
  if(event->type() == QEvent::LanguageChange) {
    m_ui->retranslateUi(this);

    /*m_boldAction->setText(tr("Bold"));
    m_italicAction->setText(tr("Italic"));
    m_underlineAction->setText(tr("Underline"));
    m_colorAction->setText(tr("Color"));
    m_smileyAction->setText(tr("Smilies"));
    m_imageAction->setText(tr("Send image"));
    m_fileAction->setText(tr("Send file"));
    m_profileAction->setText(tr("Profile"));
    m_historyAction->setText(tr("History"));*/
  }

  QWidget::changeEvent(event);
}

void Kitty::ChatTab::updateButtons()
{
  QTextCharFormat fmt = m_ui->textEdit->currentCharFormat();

  if(m_boldAction) {
    m_boldAction->setChecked(fmt.fontWeight() == QFont::Bold);
  }

  if(m_italicAction) {
    m_italicAction->setChecked(fmt.fontItalic());
  }

  if(m_underlineAction) {
    m_underlineAction->setChecked(fmt.fontUnderline());
  }

  if(m_strikethroughAction) {
    m_strikethroughAction->setChecked(fmt.fontStrikeOut());
  }
}

void Kitty::ChatTab::showColorPicker()
{
  QAction *action = qobject_cast<QAction*>(sender());
  if(action) {
    QWidget *widget = m_toolBar->widgetForAction(action);
    if(widget) {
      m_colorPicker->showAt(widget->mapToGlobal(QPoint(0, widget->height())));
    }
  }
}
