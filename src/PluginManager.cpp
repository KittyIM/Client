#include "PluginManager.h"

#include "PluginCoreImpl.h"
#include "SDK/Plugin.h"

#include <QtCore/QLibrary>
#include <QtCore/QDebug>
#include <QtCore/QDir>
#include <QtGui/QApplication>

Kitty::PluginManager::PluginManager(QObject *parent): QObject(parent)
{
}

typedef QObject *(*pluginInst)(KittySDK::PluginCore*);

void Kitty::PluginManager::load()
{
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
  foreach(QFileInfo info, files) {
    QLibrary lib(info.absoluteFilePath());
    pluginInst inst = (pluginInst)lib.resolve("inst");

    if(inst) {
      KittySDK::Plugin *plug = static_cast<KittySDK::Plugin*>(inst(new Kitty::PluginCoreImpl()));
      //plug->applySettings();

      //qDebug() << plug->info()->author();
      //qDebug() << plug->info()->email();
      //qDebug() << plug->info()->www();
    } else {
      qDebug() << "Resolve failed";
    }
  }
}
