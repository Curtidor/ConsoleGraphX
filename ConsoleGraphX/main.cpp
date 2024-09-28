#include "main_scene.h"
#include "engine.h"
#include "screen.h"

/*
Fastest Font:
	Gothic: 800-900fps/avg
*/

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
    std::array<ConsoleGraphX_Internal::RGB_CGX, 16> colors = { {
     {242, 192, 162}, // RGB(242, 192, 162)
     {233, 132, 114}, // RGB(233, 132, 114)
     {216, 35, 35},   // RGB(216, 35, 35)
     {152, 24, 60},   // RGB(152, 24, 60)
     {31, 203, 35},   // RGB(31, 203, 35)
     {18, 109, 48},   // RGB(18, 109, 48)
     {38, 221, 221},  // RGB(38, 221, 221)
     {24, 103, 160},  // RGB(24, 103, 160)
     {147, 66, 38},   // RGB(147, 66, 38)
     {108, 37, 30},   // RGB(108, 37, 30)
     {247, 226, 108}, // RGB(247, 226, 108)
     {237, 179, 41},  // RGB(237, 179, 41)
     {231, 109, 20},  // RGB(231, 109, 20)
     {242, 242, 249}, // RGB(242, 242, 249)
     {106, 95, 160},  // RGB(106, 95, 160)
     {22, 20, 35}     // RGB(22, 20, 35)
 } };


	Engine engine = Engine(400, 140, 3, 6);

    ConsoleGraphX_Internal::Screen::SetPalletColors_A(colors);

	MainScene m("Main scene");

    engine.Run();

	return 0;
}