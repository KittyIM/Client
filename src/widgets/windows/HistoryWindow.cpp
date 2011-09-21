#include "HistoryWindow.h"
#include "ui_HistoryWindow.h"

#include "AccountManager.h"
#include "SettingsWindow.h"
#include "SDK/constants.h"
#include "SDK/Contact.h"
#include "IconManager.h"
#include "Core.h"

#include <QtGui/QStandardItemModel>
#include <QtGui/QStandardItem>
#include <QtCore/QDebug>

#define qDebug() qDebug() << "[HistoryWindow]"
#define qWarning() qWarning() << "[HistoryWindow]"

using namespace Kitty;
using namespace KittySDK;

Kitty::ContactProxy::ContactProxy(QObject *parent): QSortFilterProxyModel(parent)
{
    setDynamicSortFilter(true);
    setFilterCaseSensitivity(Qt::CaseInsensitive);

    setSourceModel(new QStandardItemModel());
}

bool Kitty::ContactProxy::filterAcceptsRow(int row, const QModelIndex &parent) const
{
    QModelIndex index = sourceModel()->index(row, 0, parent);

    //root is always visible
    if(!parent.isValid()) {
        return true;
    }

    //accounts are only visible if atleast one contact is visible
    if(sourceModel()->hasChildren(index)) {
        for(int i = 0; i < sourceModel()->rowCount(index); i++) {
            if(QSortFilterProxyModel::filterAcceptsRow(i, index)) {
                return true;
            }
        }

        return false;
    }

    //contact are accepted the usual way
    return QSortFilterProxyModel::filterAcceptsRow(row, parent);
}

Kitty::HistoryWindow::HistoryWindow(QWidget *parent): QWidget(parent), m_ui(new Ui::HistoryWindow)
{
  Core *core = Core::inst();

  m_ui->setupUi(this);

  qDebug() << "Creating";

  m_proxy = new ContactProxy(this);
  m_ui->contactTree->setModel(m_proxy);

  connect(IconManager::inst(), SIGNAL(iconsUpdated()), this, SLOT(updateIcons()));
  connect(core->settingsWindow(), SIGNAL(settingsApplied()), this, SLOT(applySettings()));

  restoreGeometry(core->setting(Settings::S_HISTORYWINDOW_GEOMETRY).toByteArray());

  m_ui->dateToEdit->setDate(QDate::currentDate());
  m_ui->chatTree->header()->restoreState(core->setting(Settings::S_HISTORYWINDOW_COLUMNS, m_ui->chatTree->header()->saveState()).toByteArray());
  m_ui->filtersButton->setChecked(core->setting(Settings::S_HISTORYWINDOW_FILTERS, false).toBool());

  applySettings();
  updateIcons();
}

Kitty::HistoryWindow::~HistoryWindow()
{
  Core *core = Core::inst();

  core->setSetting(Settings::S_HISTORYWINDOW_GEOMETRY, saveGeometry());
  core->setSetting(Settings::S_HISTORYWINDOW_COLUMNS, m_ui->chatTree->header()->saveState());
  core->setSetting(Settings::S_HISTORYWINDOW_FILTERS, m_ui->filtersButton->isChecked());

  delete m_ui;
}

void Kitty::HistoryWindow::applySettings()
{

}

void Kitty::HistoryWindow::updateIcons()
{
  Core *core = Core::inst();

  m_ui->refreshButton->setIcon(core->icon(Icons::I_REFRESH));
  m_ui->exportButton->setIcon(core->icon(Icons::I_FILE));
  m_ui->importButton->setIcon(core->icon(Icons::I_FOLDER));
  m_ui->printButton->setIcon(core->icon(Icons::I_PRINTER));
  m_ui->filtersButton->setIcon(core->icon(Icons::I_FILTER));
}

void Kitty::HistoryWindow::showEvent(QShowEvent *event)
{
  Core *core = Core::inst();

  m_ui->searchEdit->clear();
  m_ui->contactSearchEdit->clear();

  QStandardItem *root = static_cast<QStandardItemModel*>(m_proxy->sourceModel())->invisibleRootItem();

  QStandardItem *conversations = new QStandardItem();
  conversations->setText(tr("Conversations"));
  conversations->setIcon(core->icon(Icons::I_FOLDER));

  /*QList<Account*> accounts = AccountManager::inst()->accounts();
  foreach(Account *account, accounts) {
    QStandardItem *accountItem = new QStandardItem();
    accountItem->setText(account->uid());
    accountItem->setIcon(core->icon(account->protocol()->protoInfo()->protoIcon()));

    foreach(const Contact *contact, account->contacts()) {
      QStandardItem *contactItem = new QStandardItem();
      contactItem->setText(contact->display());

      accountItem->appendRow(contactItem);
    }

    conversations->appendRow(accountItem);
  }*/

  root->appendRow(conversations);

  m_ui->contactTree->expandAll();

  QWidget::showEvent(event);
}

void Kitty::HistoryWindow::on_contactSearchEdit_textChanged(const QString &text)
{
  m_proxy->setFilterWildcard(text);
}
