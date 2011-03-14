#include "PluginCoreImpl.h"

#include "Core.h"

#include <QtCore/QVariant>

QVariant Kitty::PluginCoreImpl::setting(const QString &key, const QVariant &defaultValue)
{
  return Kitty::Core::inst()->setting(key, defaultValue);
}

void Kitty::PluginCoreImpl::setSetting(const QString &key, const QVariant &value)
{
  Kitty::Core::inst()->setSetting(key, value);
}
