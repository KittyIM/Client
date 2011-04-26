/**
 * \file json.h
 *
 * \author Eeli Reilin <eeli@nilier.org>,
 *         Mikko Ahonen <mikko.j.ahonen@jyu.fi>
 *         Girish Ramakrishnan <girish@forwardbias.in>
 */

#include "json.h"

#include <QtCore/QDebug>

QVariant Json::parse(const QString &json)
{
  bool success = true;

  if(!json.isNull() || !json.isEmpty()) {
    int index = 0;
    return parseValue(json, index, success);
  }	else {
    return QVariant();
  }
}

QString Json::escape(const QVariant &variant)
{
  QString str = variant.toString();
  QString res;
  res.reserve(str.length());

  for (int i = 0; i < str.length(); i++) {
    if(str[i] == '\b') {
      res += "\\b";
    } else if(str[i] == '\f') {
      res += "\\f";
    } else if(str[i] == '\n') {
      res += "\\n";
    } else if(str[i] == '\r') {
      res += "\\r";
    } else if(str[i] == '\t') {
      res += "\\t";
    } else if(str[i] == '\"') {
      res += "\\\"";
    } else if(str[i] == '\\') {
      res += "\\\\";
    } else if(str[i].unicode() > 127) {
      res += "\\u" + QString::number(str[i].unicode(), 16).rightJustified(4, '0');
    } else {
      res += str[i].toAscii();
    }
  }

  return res;
}

QString Json::stringify(const QVariant &variant, int indent)
{
  QString res;

  if((variant.type() == QVariant::List) || (variant.type() == QVariant::StringList)) {
    res = QString(" ").repeated(indent) + "[\n";

    QList<QVariant> list = variant.toList();
    foreach(QVariant var, list) {
      res += stringify(var, indent + 2);

      if(list.indexOf(var) < list.size() - 1) {
        res += ",";
      }

      res += "\n";
    }

    res += QString(" ").repeated(indent) + "]";
  } else if(variant.type() == QVariant::Map) {
    res = QString(" ").repeated(indent) + "{\n";

    QMapIterator<QString, QVariant> it(variant.toMap());
    while(it.hasNext()) {
      it.next();

      res += QString(" ").repeated(indent + 2) + "\"" + escape(it.key()) + "\": ";

      if(it.value().type() == QVariant::Map || it.value().type() == QVariant::List) {
        res += "\n";
      }

      res += stringify(it.value(), indent + 2);

      if(it.hasNext()) {
        res += ",";
      }

      res += "\n";
    }

    res += QString(" ").repeated(indent) + "}";
  } else if((variant.type() == QVariant::String) || (variant.type() == QVariant::ByteArray)) {
    res = "\"" + escape(variant) + "\"";
  } else if((variant.type() == QVariant::Double) || ((int)variant.type() == (int)QMetaType::Float)) {
    res.setNum(variant.toDouble(), 'g', 15);
  } else if(variant.type() == QVariant::Bool) {
    res = variant.toBool() ? "true" : "false";
  } else if(variant.type() == QVariant::Invalid) {
    res = "null";
  } else if(variant.type() == QVariant::ULongLong) {
    res = QString::number(variant.toULongLong());
  } else if(variant.type() == QVariant::LongLong) {
    res = QString::number(variant.toLongLong());
  } else if(variant.type() == QVariant::Int) {
    res = QString::number(variant.toInt());
  } else if(variant.type() == QVariant::UInt) {
    res = QString::number(variant.toUInt());
  } else if(variant.type() == QVariant::Char) {
    QChar c = variant.toChar();
    if(c.unicode() > 127) {
      res = "\\u" + QString::number(c.unicode(), 16).rightJustified(4, '0');
    } else {
      res.append(c.toAscii());
    }
  } else if(variant.canConvert<qlonglong>()) {
    res = QString::number(variant.toLongLong());
  } else if(variant.canConvert<QString>()) {
    res = stringify(variant);
  }

  return res;
}

QVariant Json::parseValue(const QString &json, int &index, bool &success)
{
  switch(lookAhead(json, index)) {
    case JsonTokenString:
      return parseString(json, index, success);
    break;

    case JsonTokenNumber:
      return parseNumber(json, index);
    break;

    case JsonTokenCurlyOpen:
      return parseObject(json, index, success);
     break;

    case JsonTokenSquaredOpen:
      return parseArray(json, index, success);
    break;

    case JsonTokenTrue:
    {
      nextToken(json, index);

      return true;
    }
    break;

    case JsonTokenFalse:
    {
      nextToken(json, index);

      return false;
    }
    break;

    case JsonTokenNull:
    {
      nextToken(json, index);

      return QVariant();
    }
    break;

    default:
    {
      success = false;

      return QVariant();
    }
    break;
  }
}

QVariant Json::parseObject(const QString &json, int &index, bool &success)
{
  QVariantMap map;
  int token;

  nextToken(json, index);

  bool done = false;
  while(!done) {
    token = lookAhead(json, index);

    if(token == JsonTokenNone) {
      success = false;
      return QVariantMap();
    } else if(token == JsonTokenComma) {
      nextToken(json, index);
    } else if(token == JsonTokenCurlyClose) {
      nextToken(json, index);
      return map;
    } else {
      QString name = parseString(json, index, success).toString();
      if(!success) {
        return QVariantMap();
      }

      token = nextToken(json, index);
      if(token != JsonTokenColon) {
        success = false;
        return QVariant(QVariantMap());
      }

      QVariant value = parseValue(json, index, success);
      if(!success) {
        return QVariantMap();
      }

      map[name] = value;
    }
  }

  return QVariant(map);
}

QVariant Json::parseArray(const QString &json, int &index, bool &success)
{
  QVariantList list;

  nextToken(json, index);

  bool done = false;
  while(!done) {
    int token = lookAhead(json, index);

    if(token == JsonTokenNone) {
      success = false;

      return QVariantList();
    } else if(token == JsonTokenComma) {
      nextToken(json, index);
    } else if(token == JsonTokenSquaredClose) {
      nextToken(json, index);
      break;
    } else {
      QVariant value = parseValue(json, index, success);
      if(!success) {
        return QVariantList();
      }

      list.push_back(value);
    }
  }

  return QVariant(list);
}

QVariant Json::parseString(const QString &json, int &index, bool &success)
{
  QString s;
  QChar c;

  eatWhitespace(json, index);

  c = json[index++];

  bool complete = false;
  while(!complete) {
    if(index == json.size()) {
      break;
    }

    c = json[index++];

    if(c == '\"') {
      complete = true;

      break;
    } else if(c == '\\') {
      if(index == json.size()) {
        break;
      }

      c = json[index++];

      if(c == '\"') {
        s.append('\"');
      } else if(c == '\\') {
        s.append('\\');
      } else if(c == '/') {
        s.append('/');
      } else if(c == 'b') {
        s.append('\b');
      } else if(c == 'f') {
        s.append('\f');
      } else if(c == 'n') {
        s.append('\n');
      } else if(c == 'r') {
        s.append('\r');
      } else if(c == 't') {
        s.append('\t');
      } else if(c == 'u') {
        int remainingLength = json.size() - index;

        if(remainingLength >= 4) {
          QString unicodeStr = json.mid(index, 4);

          int symbol = unicodeStr.toInt(0, 16);

          s.append(QChar(symbol));

          index += 4;
        } else {
          break;
        }
      }
    } else {
      s.append(c);
    }
  }

  if(!complete) {
    success = false;

    return QVariant();
  }

  return QVariant(s);
}

QVariant Json::parseNumber(const QString &json, int &index)
{
  eatWhitespace(json, index);

  int lastIndex = Json::lastIndexOfNumber(json, index);
  int charLength = (lastIndex - index) + 1;
  QString numberStr = json.mid(index, charLength);

  index = lastIndex + 1;

  return QVariant(numberStr);
}

int Json::lastIndexOfNumber(const QString &json, int index)
{
  int lastIndex;

  for(lastIndex = index; lastIndex < json.size(); lastIndex++) {
    if(QString("0123456789+-.eE").indexOf(json[lastIndex]) == -1) {
      break;
    }
  }

  return lastIndex -1;
}

void Json::eatWhitespace(const QString &json, int &index)
{
  for(; index < json.size(); index++) {
    if(QString(" \t\n\r").indexOf(json[index]) == -1) {
      break;
    }
  }
}

int Json::lookAhead(const QString &json, int index)
{
  int saveIndex = index;

  return nextToken(json, saveIndex);
}

int Json::nextToken(const QString &json, int &index)
{
  eatWhitespace(json, index);

  if(index == json.size()) {
    return JsonTokenNone;
  }

  QChar c = json[index];
  index++;

  switch(c.toAscii()) {
    case '{':
      return JsonTokenCurlyOpen;
    break;

    case '}':
      return JsonTokenCurlyClose;
    break;

    case '[':
      return JsonTokenSquaredOpen;
    break;

    case ']':
      return JsonTokenSquaredClose;
    break;

    case ',':
      return JsonTokenComma;
    break;

    case '"':
      return JsonTokenString;
    break;

    case '0':
    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    case '7':
    case '8':
    case '9':
    case '-':
      return JsonTokenNumber;
    break;

    case ':':
      return JsonTokenColon;
    break;
  }

  index--;

  int remainingLength = json.size() - index;
  if(remainingLength >= 5) {
    if(json.mid(index, 5) == "false") {
      index += 5;

      return JsonTokenFalse;
    }
  }

  if(remainingLength >= 4) {
    if(json.mid(index, 4) == "true") {
      index += 4;

      return JsonTokenTrue;
    } else if(json.mid(index, 4) == "null") {
      index += 4;

      return JsonTokenNull;
    }
  }

  return JsonTokenNone;
}
