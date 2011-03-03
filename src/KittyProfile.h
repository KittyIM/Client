#ifndef KITTYPROFILE_H
#define KITTYPROFILE_H

#include <QtCore/QObject>
#include <QtCore/QString>

class KittyProfile: public QObject
{
  Q_OBJECT

  public:
    explicit KittyProfile(QObject *parent = 0);

    void load(QString name);

    QString name() { return m_name; }
    bool isLoaded() { return !m_name.isEmpty(); }
    bool hasPassword() { return false; }

  private:
    QString m_name;
};

#endif // KITTYPROFILE_H
