#include "App.h"

int main(int argc, char *argv[])
{
  return (new Kitty::App(argc, argv))->exec();
}
