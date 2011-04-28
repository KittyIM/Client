#include "ChatTheme.h"

#include <QtCore/QDebug>
#include <QtCore/QDir>
#include <QtGui/QApplication>


Kitty::ChatTheme::ChatTheme(const QString &name, QObject *parent): QObject(parent)
{
  load(name);
}


QString Kitty::ChatTheme::getCode(const Type &type) const
{
  QString code = m_codes.value(type);

  switch(type) {
    case IncomingNextContent:
    {
      if(code.isEmpty()) {
        return getCode(IncomingContent);
      }
    }
    break;

    case OutgoingContent:
    {
      if(code.isEmpty()) {
        return getCode(IncomingContent);
      }
    }
    break;

    case OutgoingNextContent:
    {
      if(code.isEmpty()) {
        return getCode(OutgoingContent);
      }
    }
    break;

    case IncomingContext:
    {
      if(code.isEmpty()) {
        return getCode(IncomingContent);
      }
    }
    break;

    case OutgoingContext:
    {
      if(code.isEmpty()) {
        return getCode(OutgoingContent);
      }
    }
    break;

    case IncomingNextContext:
    {
      if(code.isEmpty()) {
        return getCode(IncomingNextContent);
      }
    }
    break;

    case OutgoingNextContext:
    {
      if(code.isEmpty()) {
        return getCode(OutgoingNextContent);
      }
    }
    break;

    case IncomingContent:
    case Status:
    {
      if(code.isEmpty()) {
        return getCode(Content);
      }
    }
    break;

    default:
    break;
  }

  return code;
}

void Kitty::ChatTheme::load(const QString &name)
{
  loadDefaults();

  QDir dir(qApp->applicationDirPath() + "/themes/chat/" + name);
  if(dir.exists()) {
    QMap<Type, QString> files;
    files.insert(Footer, "Footer.html");
    files.insert(Header, "Header.html");
    files.insert(Content, "Content.html");
    files.insert(Status, "Status.html");
    files.insert(IncomingContent, "Incoming/Content.html");
    files.insert(IncomingNextContent, "Incoming/NextContent.html");
    files.insert(IncomingContext, "Incoming/Context.html");
    files.insert(IncomingNextContext, "Incoming/NextContext.html");
    files.insert(OutgoingContent, "Outgoing/Content.html");
    files.insert(OutgoingNextContent, "Outgoing/NextContent.html");
    files.insert(OutgoingContext, "Outgoing/Context.html");
    files.insert(OutgoingNextContext, "Outgoing/NextContext.html");

    QMapIterator<Type, QString> it(files);
    while(it.hasNext()) {
      it.next();

      if(QFile::exists(dir.absolutePath() + "/" + it.value())) {
        loadFile(it.key(), dir.absolutePath() + "/" + it.value());
      }
    }
  }
}

void Kitty::ChatTheme::loadFile(const ChatTheme::Type &type, const QString &fileName)
{
  QFile file(fileName);
  if(file.open(QFile::ReadOnly)) {
    m_codes.insert(type, file.readAll());

    file.close();
  } else {
    qWarning() << "Couldn't open file" << fileName;
  }
}

void Kitty::ChatTheme::loadDefaults()
{
  m_codes.clear();

  QMap<Type, QString> defaults;
  defaults.insert(Header, ":/chat/Header.html");
  defaults.insert(Status, ":/chat/Status.html");
  defaults.insert(IncomingContent, ":/chat/Incoming/Content.html");
  defaults.insert(IncomingNextContent, ":/chat/Incoming/NextContent.html");
  defaults.insert(OutgoingContent, ":/chat/Outgoing/Content.html");
  defaults.insert(OutgoingNextContent, ":/chat/Outgoing/NextContent.html");

  QMapIterator<Type, QString> it(defaults);
  while(it.hasNext()) {
    it.next();

    loadFile(it.key(), it.value());
  }
}
