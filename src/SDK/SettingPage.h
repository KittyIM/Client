#ifndef SDKSETTINGPAGE_H
#define SDKSETTINGPAGE_H

#include <QtGui/QWidget>

namespace KittySDK
{
  class SettingPage: public QWidget
  {
    Q_OBJECT
    Q_PROPERTY(QString name READ name WRITE setName)

    public:
      SettingPage(QWidget *parent = 0): QWidget(parent) { }

      QString name() { return m_name; }
      void setName(const QString &name) { m_name = name; }

      virtual void apply() { }
      virtual void reset() { }

    private:
      QString m_name;
  };
}

#endif // SDKSETTINGPAGE_H
