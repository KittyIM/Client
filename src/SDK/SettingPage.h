#ifndef SDKSETTINGPAGE_H
#define SDKSETTINGPAGE_H

#include <QtGui/QWidget>

namespace KittySDK
{
  class SettingPage: public QWidget
  {
    Q_OBJECT
    Q_PROPERTY(QString name READ name WRITE setName)
    Q_PROPERTY(QString icon READ icon WRITE setIcon)

    public:
      SettingPage(QWidget *parent = 0): QWidget(parent) { }

      QString name() { return m_name; }
      void setName(const QString &name) { m_name = name; }

      QString icon() { return m_icon; }
      void setIcon(const QString &icon) { m_icon = icon; }

      virtual void apply() { }
      virtual void reset() { }

    private:
      QString m_name;
      QString m_icon;
  };
}

#endif // SDKSETTINGPAGE_H
