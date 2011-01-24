#include "KittyCore.h"
#include "wndMain.h"

#include <QtCore/QMutex>

KittyCore *KittyCore::m_inst = 0;

KittyCore::KittyCore()
{
  m_wndMain = 0;
}

KittyCore::~KittyCore()
{
  if(m_wndMain) {
    delete m_wndMain;
  }
}

KittyCore* KittyCore::inst()
{
  static QMutex mutex;

  if(!m_inst) {
    mutex.lock();
    m_inst = new KittyCore();
    mutex.unlock();
  }

  return m_inst;
}

void KittyCore::destroy()
{
  static QMutex mutex;

  mutex.lock();
  delete m_inst;
  m_inst = 0;
  mutex.unlock();
}

wndMain *KittyCore::getWndMain()
{
  if(!m_wndMain) {
    m_wndMain = new wndMain();
  }

  return m_wndMain;
}

void KittyCore::showWndMain()
{
  getWndMain()->show();
}
