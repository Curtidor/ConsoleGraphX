#include "../ConsoleGraphX/application.h"
#include "../ConsoleGraphX/palette.h"
#include "../ConsoleGraphX/screen.h"
#include "main_scene.h"

/*
RGB(242, 192, 162)
RGB(233, 132, 114)
RGB(216, 35, 35)
RGB(152, 24, 60)
RGB(31, 203, 35)
RGB(18, 109, 48)
RGB(38, 221, 221)
RGB(24, 103, 160)
RGB(147, 66, 38)
RGB(108, 37, 30)
RGB(247, 226, 108)
RGB(237, 179, 41)
RGB(231, 109, 20)
RGB(242, 242, 249)
RGB(106, 95, 160)
RGB(22, 20, 35)
*/


using namespace ConsoleGraphX;

int main()
{
	Application mainApplication = Application();

	Palette& defaultPalette = Palette::DefaultPalette();
	ConsoleGraphX_Internal::Screen::SetPalletColors_A(defaultPalette);

	MainScene m("Main scene");

	mainApplication.Run();

	return 0;
}