#include "ChatTab.h"
#include "ui_ChatTab.h"

#include "SDK/constants.h"
#include "IconManager.h"
#include "Core.h"

#include <QtGui/QToolBar>

using namespace KittySDK;

Kitty::ChatTab::ChatTab(QWidget *parent): QWidget(parent), m_ui(new Ui::ChatTab)
{
  m_ui->setupUi(this);

  Kitty::Core *core = Kitty::Core::inst();

  m_toolBar = new QToolBar(this);
  m_toolBar->setIconSize(QSize(16, 16));
  m_toolBar->setStyleSheet("QToolBar { border: 0; }");
  m_ui->verticalLayout->addWidget(m_toolBar);

  m_boldAction = m_toolBar->addAction(core->icon(Icons::I_BOLD), tr("Bold"));
  m_italicAction = m_toolBar->addAction(core->icon(Icons::I_ITALIC), tr("Italic"));
  m_underlineAction = m_toolBar->addAction(core->icon(Icons::I_UNDERLINE), tr("Underline"));
  m_toolBar->addSeparator();
  m_colorAction  = m_toolBar->addAction(core->icon(Icons::I_COLOR), tr("Color"));
  m_smileyAction = m_toolBar->addAction(core->icon(Icons::I_SMILEY), tr("Smilies"));
  m_toolBar->addSeparator();
  m_imageAction = m_toolBar->addAction(core->icon(Icons::I_IMAGE), tr("Send image"));
  m_fileAction = m_toolBar->addAction(core->icon(Icons::I_FILE), tr("Send file"));
  m_toolBar->addSeparator();
  m_profileAction = m_toolBar->addAction(core->icon(Icons::I_PROFILE), tr("Profile"));
  m_historyAction = m_toolBar->addAction(core->icon(Icons::I_HISTORY), tr("History"));
}

Kitty::ChatTab::~ChatTab()
{
  delete m_ui;
}

void Kitty::ChatTab::updateIcons()
{
  Kitty::Core *core = Kitty::Core::inst();

  m_boldAction->setIcon(core->icon(Icons::I_BOLD));
  m_italicAction->setIcon(core->icon(Icons::I_ITALIC));
  m_underlineAction->setIcon(core->icon(Icons::I_UNDERLINE));
  m_colorAction->setIcon(core->icon(Icons::I_COLOR));
  m_smileyAction->setIcon(core->icon(Icons::I_SMILEY));
  m_imageAction->setIcon(core->icon(Icons::I_IMAGE));
  m_fileAction->setIcon(core->icon(Icons::I_FILE));
  m_profileAction->setIcon(core->icon(Icons::I_PROFILE));
  m_historyAction->setIcon(core->icon(Icons::I_HISTORY));
}

void Kitty::ChatTab::changeEvent(QEvent *event)
{
  if(event->type() == QEvent::LanguageChange) {
    m_ui->retranslateUi(this);

    m_boldAction->setText(tr("Bold"));
    m_italicAction->setText(tr("Italic"));
    m_underlineAction->setText(tr("Underline"));
    m_colorAction->setText(tr("Color"));
    m_smileyAction->setText(tr("Smilies"));
    m_imageAction->setText(tr("Send image"));
    m_fileAction->setText(tr("Send file"));
    m_profileAction->setText(tr("Profile"));
    m_historyAction->setText(tr("History"));
  }

  QWidget::changeEvent(event);
}
