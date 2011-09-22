#include "EmoticonManager.h"

#include "SDK/constants.h"
#include "EmoticonPack.h"
#include "Core.h"

using namespace Kitty;
using namespace KittySDK;

Kitty::EmoticonManager::~EmoticonManager()
{
  qDeleteAll(m_packs);
}

QString Kitty::EmoticonManager::parse(const QString &text)
{
  return "";
}

void Kitty::EmoticonManager::load()
{
  qDeleteAll(m_packs);

  //Core *core = Core::inst();
}

void Kitty::EmoticonManager::applySettings()
{
  load();
}
