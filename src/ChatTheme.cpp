#include "ChatTheme.h"

#include <QtCore/QDebug>
#include <QtCore/QDir>
#include <QtGui/QApplication>

#define qDebug() qDebug() << "[ChatTheme]"
#define qWarning() qWarning() << "[ChatTheme]"

Kitty::ChatTheme::ChatTheme(const QString &name, QObject *parent): QObject(parent), m_name(name)
{
	if(name.isEmpty() || (name == tr("Default"))) {
		m_name = tr("Default");
		loadDefaults();
	} else {
		load(name);
	}
}

QString Kitty::ChatTheme::code(const Type &type) const
{
	QString result = m_codes.value(type);

	switch(type) {
		case IncomingNextContent:
		{
			if(result.isEmpty()) {
				return code(IncomingContent);
			}
		}
		break;

		case OutgoingContent:
		{
			if(result.isEmpty()) {
				return code(IncomingContent);
			}
		}
		break;

		case OutgoingNextContent:
		{
			if(result.isEmpty()) {
				return code(OutgoingContent);
			}
		}
		break;

		case IncomingContext:
		{
			if(result.isEmpty()) {
				return code(IncomingContent);
			}
		}
		break;

		case OutgoingContext:
		{
			if(result.isEmpty()) {
				return code(OutgoingContent);
			}
		}
		break;

		case IncomingNextContext:
		{
			if(result.isEmpty()) {
				return code(IncomingNextContent);
			}
		}
		break;

		case OutgoingNextContext:
		{
			if(result.isEmpty()) {
				return code(OutgoingNextContent);
			}
		}
		break;

		case IncomingContent:
		case Status:
		{
			if(result.isEmpty()) {
				return code(Content);
			}
		}
		break;

		default:
		break;
	}

	return result;
}

QString Kitty::ChatTheme::iconPath(const IconType &type)
{
	switch(type) {
		case Incoming:
		return qApp->applicationDirPath() + "/themes/chat/" + m_name + "/Incoming/buddy_icon.png";
		break;

		case Outgoing:
		return qApp->applicationDirPath() + "/themes/chat/" + m_name + "/Outgoing/buddy_icon.png";
		break;

		default:
		return "";
		break;
	}
}

void Kitty::ChatTheme::load(const QString &name)
{
	m_codes.clear();

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
