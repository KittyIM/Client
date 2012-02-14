#include "EmoticonManager.h"

#include "EmoticonPack.h"
#include "Core.h"

#include <SDKConstants.h>

namespace Kitty
{

EmoticonManager::~EmoticonManager()
{
	qDeleteAll(m_packs);
}

QString EmoticonManager::parse(const QString &text)
{
	return "";
}

void EmoticonManager::load()
{
	qDeleteAll(m_packs);

	//Core *core = Core::inst();
}

void EmoticonManager::applySettings()
{
	load();
}

}
