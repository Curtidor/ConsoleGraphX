#include <iostream>
#include "main_scene.h"
#include "render_testing.h"

/*
Fastest Font:
	Gothic: 800-900fps/avg
*/

int main()
{
	Debugger debugger = Debugger(10);
	Engine::InitializeEngine(300, 150, 3, 6, &debugger);

	MainScene* m = new MainScene("Main scene");

	Engine::Run();


	return 0;
}