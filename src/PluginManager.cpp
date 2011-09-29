#include "PluginManager.h"

#include "ProtocolManager.h"
#include "PluginCoreImpl.h"
#include "SDK/Protocol.h"

#include <QtCore/QLibrary>
#include <QtCore/QDebug>
#include <QtCore/QDir>
#include <QtGui/QApplication>

#define qDebug() qDebug() << "[PluginManager]"
#define qWarning() qWarning() << "[PluginManager]"

using namespace Kitty;
using namespace KittySDK;

typedef QObject *(*pluginInst)(PluginCore*);

Kitty::Plugin::Plugin(const QString &fileName): m_fileName(fileName)
{
  m_loaded = false;
  m_inited = false;

  QLibrary lib(m_fileName);
  pluginInst inst = (pluginInst)lib.resolve("inst");

  if(inst) {
    if(m_plugin = dynamic_cast<KittySDK::Plugin*>(inst(new PluginCoreImpl()))) {
      if(m_plugin->type() == KittySDK::Plugin::Type) {
        //nothing for now
      } else if(m_plugin->type() == Protocol::Type) {
        if(Protocol *prot = dynamic_cast<Protocol*>(m_plugin)) {
          Kitty::ProtocolManager::inst()->add(prot);
        } else {
          qWarning() << "Could not cast to protocol for file" << QFileInfo(m_fileName).fileName();
        }
      } else {
        qWarning() << "Unknown type for file" << QFileInfo(m_fileName).fileName();
      }
    } else {
      qWarning() << "Could not cast to Plugin for file" << QFileInfo(m_fileName).fileName();
    }
  } else {
    qWarning() << "Resolve failed" << QFileInfo(m_fileName).fileName();
  }
}

void Kitty::Plugin::init()
{
  if(!isInited()) {
    m_plugin->init();
    m_inited = true;
  }
}

void Kitty::Plugin::load()
{
  if(!isLoaded()) {
    init();

    m_plugin->load();
    m_loaded = true;
  } else {
    qWarning() << "Trying to load already loaded plugin" << QFileInfo(m_fileName).fileName();
  }
}

void Kitty::Plugin::unload()
{

}

const QList<Kitty::Plugin*> &Kitty::PluginManager::plugins() const
{
  return m_plugins;
}

Kitty::Plugin *Kitty::PluginManager::pluginByName(const QString &name) const
{
  foreach(Plugin *plugin, m_plugins) {
    if(plugin->isLoaded()) {
      if(plugin->plugin() && plugin->plugin()->info()) {
        if(plugin->plugin()->info()->name() == name) {
          return plugin;
        }
      }
    }
  }

  return 0;
}

Kitty::Plugin *Kitty::PluginManager::pluginByFileName(const QString &fileName) const
{
  foreach(Plugin *plugin, m_plugins) {
    if(QFileInfo(plugin->fileName()).fileName() == fileName) {
      return plugin;
    }
  }

  return 0;
}

void Kitty::PluginManager::execAction(const QString &pluginName, const QString &name, const QMap<QString, QVariant> &args)
{
  foreach(Plugin *plugin, m_plugins) {
    if(pluginName.isEmpty() || (plugin->plugin()->info()->name() == pluginName)) {
      plugin->plugin()->execAction(name, args);
    }
  }
}

void Kitty::PluginManager::load()
{
  qDebug() << "Starting";
  QDir dir(qApp->applicationDirPath() + "/plugins");
  QStringList filter;

#if defined(Q_OS_WIN)
  filter.append("*.dll");
#elif defined(Q_OS_MAC)
  filter.append("*.dylib");
  filter.append("*.so");
#elif defined(Q_OS_LINUX)
  filter.append("*.so");
#endif

  QFileInfoList files = dir.entryInfoList(filter, QDir::Files);
  foreach(const QFileInfo &info, files) {
    qDebug() << "Found file: " << info.fileName();

    Plugin *plug = new Plugin(info.absoluteFilePath());
    plug->load();
    m_plugins.append(plug);
  }

  emit allPluginsLoaded();
}

Kitty::PluginManager::~PluginManager()
{
  qDeleteAll(m_plugins);
}
