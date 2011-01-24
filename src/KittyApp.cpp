#include "KittyApp.h"
#include "KittyCore.h"

KittyApp::KittyApp(int &argc, char **argv): QApplication(argc, argv)
{
  KittyCore::inst()->showWndMain();
}
