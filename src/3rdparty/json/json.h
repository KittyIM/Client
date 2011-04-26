/**
 * \file json.h
 *
 * \author Eeli Reilin <eeli@nilier.org>,
 *         Mikko Ahonen <mikko.j.ahonen@jyu.fi>
 */

#ifndef JSON_H
#define JSON_H

#include <QtCore/QVariant>
#include <QtCore/QString>

class Json
{
  public:
    enum JsonToken
    {
      JsonTokenNone          = 0,
      JsonTokenCurlyOpen     = 1,
      JsonTokenCurlyClose    = 2,
      JsonTokenSquaredOpen   = 3,
      JsonTokenSquaredClose  = 4,
      JsonTokenColon         = 5,
      JsonTokenComma         = 6,
      JsonTokenString        = 7,
      JsonTokenNumber        = 8,
      JsonTokenTrue          = 9,
      JsonTokenFalse         = 10,
      JsonTokenNull          = 11
    };

  public:
    static QVariant parse(const QString &json);

    static QString escape(const QVariant &variant);
    static QString stringify(const QVariant &variant, int indent = 0);

  private:
    static QVariant parseValue(const QString &json, int &index, bool &success);
    static QVariant parseObject(const QString &json, int &index, bool &success);
    static QVariant parseArray(const QString &json, int &index, bool &success);
    static QVariant parseString(const QString &json, int &index, bool &success);
    static QVariant parseNumber(const QString &json, int &index);
    static int lastIndexOfNumber(const QString &json, int index);
    static void eatWhitespace(const QString &json, int &index);
    static int lookAhead(const QString &json, int index);
    static int nextToken(const QString &json, int &index);
};

#endif //JSON_H
