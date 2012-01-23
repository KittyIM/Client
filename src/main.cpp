#include "App.h"

using namespace Kitty;

int main(int argc, char *argv[])
{
	return (new App(argc, argv))->exec();
}
