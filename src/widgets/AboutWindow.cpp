#include "AboutWindow.h"
#include "ui_AboutWindow.h"

#include "constants.h"

using namespace Kitty;

AboutWindow::AboutWindow(QWidget *parent): QDialog(parent), m_ui(new Ui::AboutWindow)
{
  m_ui->setupUi(this);
}

AboutWindow::~AboutWindow()
{
  delete m_ui;
}

void AboutWindow::showEvent(QShowEvent *event)
{
  QDialog::showEvent(event);

  QMap<QString, QString> programmers;
  programmers.insert("Artur \"arturo182\" Pacholec", "arturo182@tlen.pl");

  QString text = "<b>KittyIM v" + QString(Constants::VERSION) + "</b><br>" +
                 "<br>" +
                 "<i>" + tr("Programming:") + "</i><br>";

  QMapIterator<QString, QString> i(programmers);
  while(i.hasNext()) {
    i.next();
    text.append(QString("%1 &lt;<a href=\"mailto:%2\">%2</a>&gt;<br>").arg(i.key()).arg(i.value()));
  }

  m_ui->aboutText->setText(text);
}