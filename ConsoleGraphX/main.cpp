#include "main_scene.h"
#include "debugger.h"
#include "engine.h"

/*
Fastest Font:
	Gothic: 800-900fps/avg
*/

int main()
{
	ConsoleGraphX_Interal::Debugger debugger(10);
	
	Engine::InitializeEngine(300, 120, 3, 6, debugger);

	MainScene m("Main scene");
	
	Engine::Run();


	return 0;
}