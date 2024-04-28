#include "main_scene.h"
#include "debugger.h"
#include "engine.h"

/*
Fastest Font:
	Gothic: 800-900fps/avg
*/

int main()
{
	ConsoleGraphX_Interal::Debugger debugger = ConsoleGraphX_Interal::Debugger(10);
	Engine::InitializeEngine(270, 100, 3, 6, &debugger);

	MainScene* m = new MainScene("Main scene");
	
	Engine::Run();


	return 0;
}