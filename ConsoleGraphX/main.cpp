#include "main_scene.h"
#include "debugger.h"
#include "engine.h"

/*
Fastest Font:
	Gothic: 800-900fps/avg
*/

using namespace ConsoleGraphX;

int main()
{
	ConsoleGraphX_Internal::Debugger debugger(L"Debugger");

	Engine::Initialize(300, 120, 3, 6, debugger);

	MainScene m("Main scene");

	Engine::Run();

	return 0;
}