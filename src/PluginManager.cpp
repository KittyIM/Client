#include "PluginManager.h"

#include "ProtocolManager.h"
#include "PluginCoreImpl.h"
#include "SDK/Protocol.h"

#include <QtCore/QLibrary>
#include <QtCore/QDebug>
#include <QtCore/QDir>
#include <QtGui/QApplication>

typedef QObject *(*pluginInst)(KittySDK::PluginCore*);

void Kitty::PluginManager::load()
{
  qDebug() << "PluginManager starting";
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
    qDebug() << "  Found plugin: " << info.fileName();
    QLibrary lib(info.absoluteFilePath());
    pluginInst inst = (pluginInst)lib.resolve("inst");

    if(inst) {
      KittySDK::Plugin *plug = dynamic_cast<KittySDK::Plugin*>(inst(new Kitty::PluginCoreImpl()));
      if(plug) {
        KittySDK::Protocol *prot = dynamic_cast<KittySDK::Protocol*>(plug);
        if(prot) {
          Kitty::ProtocolManager::inst()->add(prot);
        } else {
          m_plugins.append(plug);
        }
      } else {
        qWarning() << "Could not cast to Plugin for file" << info.fileName();
      }
      //plug->applySettings();

      //qDebug() << "  Plugin's name: " << plug->info()->name();
      //qDebug() << plug->info()->author();
      //qDebug() << plug->info()->email();
      //qDebug() << plug->info()->www();
    } else {
      qWarning() << "Resolve failed";
    }
  }
}
