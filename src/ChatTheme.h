#ifndef CHATTHEME_H
#define CHATTHEME_H

#include <QtCore/QObject>
#include <QtCore/QString>
#include <QtCore/QMap>

namespace Kitty
{
  class ChatTheme: public QObject
  {
    Q_OBJECT

    public:
      enum Type
      {
        Footer = 0,
        Header,
        Content,
        Status,
        IncomingContent,
        IncomingNextContent,
        IncomingContext,
        IncomingNextContext,
        OutgoingContent,
        OutgoingNextContent,
        OutgoingContext,
        OutgoingNextContext
      };

    public:
      ChatTheme(const QString &name = QString(), QObject *parent = 0);

      QString getCode(const Type &type) const;
      void load(const QString &name);

    private:
      void loadDefaults();
      void loadFile(const Type &type, const QString &fileName);

    private:
      QMap<Type, QString> m_codes;
  };
}

#endif // CHATTHEME_H