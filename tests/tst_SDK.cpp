#include <QtCore/QString>
#include <QtTest>

#include <SDKConstants.h>
#include <IMessage.h>
#include <IContact.h>
#include <IChat.h>

class MockProtocol: public KittySDK::IProtocol
{
	public:
		MockProtocol(KittySDK::IPluginCore *core): IProtocol(core)
		{ }

		KittySDK::IAccount *newAccount(const QString &uid)
		{
			return 0;
		}

		QDialog *editDialog(KittySDK::IAccount *account = 0)
		{
			return 0;
		}
};

class SDKTest: public QObject
{
	Q_OBJECT

	private slots:
		void testMessageConstructor();
		void testMessageProperties();
		void testContactConstructor();
		void testContactProperties();
		void testAccountConstructor();
		void testAccountProperties();
		void testProtocolConstructor();
		void testProtocolProperties();
		void testPluginInfoConstructor();
		void testPluginInfoProperties();
		void testProtocolInfoConstructor();
		void testProtocolInfoProperties();
		void testChatConstructor();
		void testChatConstructor2();
};

void SDKTest::testMessageConstructor()
{
	KittySDK::IContact cnt("", 0);

	KittySDK::IMessage msg(&cnt, &cnt);
	QVERIFY(msg.chat() == 0);
	QVERIFY(msg.from() == &cnt);
	QVERIFY(msg.to().count() == 1);
	QVERIFY(msg.singleTo() == &cnt);
	QVERIFY(msg.body().isEmpty() == true);
	QVERIFY(msg.direction() == KittySDK::IMessage::Outgoing);
	QVERIFY(msg.timeStamp().isNull() == false);
}

void SDKTest::testMessageProperties()
{
	KittySDK::IContact cnt("", 0);

	KittySDK::IMessage msg(&cnt, &cnt);

	msg.setBody("foo bar");
	QVERIFY(msg.body() == "foo bar");

	msg.setTimeStamp(QDateTime(QDate(2012, 01, 01), QTime(12, 34, 56)));
	QVERIFY(msg.timeStamp() == QDateTime(QDate(2012, 01, 01), QTime(12, 34, 56)));

	msg.setDirection(KittySDK::IMessage::System);
	QVERIFY(msg.direction() == KittySDK::IMessage::System);

	QVERIFY(msg.to().first() == msg.singleTo());
}

void SDKTest::testContactConstructor()
{
	KittySDK::IAccount acc("bar@example.com/acc", 0);

	QString uid = "foo@example.com/cnt";

	KittySDK::IContact cnt(uid, &acc);
	QVERIFY(cnt.uid() == uid);
	QVERIFY(cnt.status() == KittySDK::IProtocol::Offline);
	QVERIFY(cnt.account() == &acc);
	QVERIFY(cnt.protocol() == 0);
}

void SDKTest::testContactProperties()
{
	KittySDK::IAccount acc("bar@example.com/acc", 0);

	KittySDK::IContact cnt("foo@example.com/cnt", &acc);

	QString display = "foo.bar";
	cnt.setDisplay(display);
	QVERIFY(cnt.display() == display);

	QString group = "example";
	cnt.setGroup(group);
	QVERIFY(cnt.group() == group);

	KittySDK::IProtocol::Status status = KittySDK::IProtocol::Online;
	cnt.setStatus(status);
	QVERIFY(cnt.status() == status);

	QString description = "Example description.";
	cnt.setDescription(description);
	QVERIFY(cnt.description() == description);

	QString lastName = "FooBar";
	cnt.setData(KittySDK::ContactInfos::I_LASTNAME, lastName);
	QVERIFY(cnt.data(KittySDK::ContactInfos::I_FIRSTNAME).isNull() == true);
	QVERIFY(cnt.data(KittySDK::ContactInfos::I_LASTNAME) == QVariant(lastName));
}

void SDKTest::testAccountConstructor()
{
	MockProtocol proto(0);

	QString uid = "bar@example.com/acc";

	KittySDK::IAccount acc(uid, &proto);
	QVERIFY(acc.uid() == uid);
	QVERIFY(acc.me() == 0);
	QVERIFY(acc.protocol() == &proto);
	QVERIFY(acc.status() == KittySDK::IProtocol::Offline);
	QVERIFY(acc.description().isEmpty() == true);
	QVERIFY(acc.description().isNull() == true);
	QVERIFY(acc.contacts().isEmpty() == true);
	QVERIFY(acc.saveSettings().isEmpty() == true);
	QVERIFY(acc.statusMenu() == 0);
}

void SDKTest::testAccountProperties()
{
	MockProtocol proto(0);

	KittySDK::IAccount acc("bar@example.com/acc", &proto);

	QString password = "FooBar";
	acc.setPassword(password);
	QVERIFY(acc.password() == password);
}

void SDKTest::testProtocolConstructor()
{
	MockProtocol proto(0);
	QVERIFY(proto.type() == KittySDK::IProtocol::Type);
	QVERIFY(proto.core() == 0);
	QVERIFY(proto.protoInfo() == 0);
	QVERIFY(proto.abilities() == 0);
}

void SDKTest::testProtocolProperties()
{
	MockProtocol proto(0);

	KittySDK::IProtocol::Abilities abilities = KittySDK::IProtocol::TextStandard | KittySDK::IProtocol::ChangeStatus;
	proto.setAbilities(abilities);
	QVERIFY(proto.abilities() == abilities);
}

void SDKTest::testPluginInfoConstructor()
{
	KittySDK::IPluginInfo info;
	QVERIFY(info.author().isEmpty() == true);
	QVERIFY(info.author().isNull() == true);
	QVERIFY(info.email().isEmpty() == true);
	QVERIFY(info.email().isNull() == true);
	QVERIFY(info.id().isEmpty() == true);
	QVERIFY(info.id().isNull() == true);
	QVERIFY(info.name().isEmpty() == true);
	QVERIFY(info.name().isNull() == true);
	QVERIFY(info.url().isEmpty() == true);
	QVERIFY(info.url().isNull() == true);
	QVERIFY(info.version().isEmpty() == true);
	QVERIFY(info.version().isNull() == true);
}

void SDKTest::testPluginInfoProperties()
{
	KittySDK::IPluginInfo info;

	QString author = "foo bar";
	info.setAuthor(author);
	QVERIFY(info.author() == author);

	QString email = "me@example.com";
	info.setEmail(email);
	QVERIFY(info.email() == email);

	QString id = "pluginId";
	info.setId(id);
	QVERIFY(info.id() == id);

	QString name = "FooBar";
	info.setName(name);
	QVERIFY(info.name() == name);

	QString url = "http://www.example.com";
	info.setURL(url);
	QVERIFY(info.url() == url);

	QString version = "1.2.3";
	info.setVersion(version);
	QVERIFY(info.version() == version);
}

void SDKTest::testProtocolInfoConstructor()
{
	KittySDK::IProtocolInfo info;
	QVERIFY(info.protoName().isEmpty() == true);
	QVERIFY(info.protoName().isNull() == true);
	QVERIFY(info.protoIcon().isEmpty() == true);
	QVERIFY(info.protoIcon().isNull() == true);
}

void SDKTest::testProtocolInfoProperties()
{
	KittySDK::IProtocolInfo info;

	QString name = "FooBar";
	info.setProtoName(name);
	QVERIFY(info.protoName() == name);

	QString icon = "Foo.Bar";
	info.setProtoIcon(icon);
	QVERIFY(info.protoIcon() == icon);
}

void SDKTest::testChatConstructor()
{
	KittySDK::IAccount acc("foo@example.com/acc", 0);
	KittySDK::IContact me("foo@example.com/cnt", &acc);
	QList<KittySDK::IContact*> contacts;

	KittySDK::IChat chat(&me, contacts);
	QVERIFY(chat.account() == &acc);
	QVERIFY(chat.contacts().count() == 0);
	QVERIFY(chat.id().isEmpty() == true);
	QVERIFY(chat.id().isNull() == true);
	QVERIFY(chat.me() == &me);
	QVERIFY(chat.protocol() == 0);
}

void SDKTest::testChatConstructor2()
{
	KittySDK::IAccount acc("foo@example.com/acc", 0);
	KittySDK::IContact me("foo@example.com/cnt", &acc);
	KittySDK::IContact cnt("bar@example.com/cnt", &acc);
	QList<KittySDK::IContact*> contacts;
	contacts << &cnt;
	QString id = "a1b2c3d4e5f6";

	KittySDK::IChat chat(&me, contacts, id);
	QVERIFY(chat.id() == id);
	QVERIFY(chat.contacts().count() == 1);
	QVERIFY(chat.contacts().first() == &cnt);
}

QTEST_APPLESS_MAIN(SDKTest)
#include "tst_SDK.moc"
