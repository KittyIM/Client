#include "Profile.h"

#include "widgets/DebugWindow.h"
#include "SDK/constants.h"
#include "XmlSettings.h"
#include "Core.h"

#include <QtCore/QDebug>

using namespace Kitty;
using namespace KittySDK;

Profile::Profile(QObject *parent): QObject(parent)
{
  m_settings = 0;
}

Profile::~Profile()
{
  if(isLoaded()) {
    settings()->setValue("Kitty.DebugWindow.Geometry", DebugWindow::inst()->saveGeometry());
  }
}

void Profile::load(const QString &name)
{
  Core *core = Core::inst();

  m_settings = new XmlSettings(core->profilesDir() + name + "/settings.xml", this);

  m_name = name;
}

bool Profile::hasPassword()
{
  return !settings()->value("Profile.Password").toString().isEmpty();
}
