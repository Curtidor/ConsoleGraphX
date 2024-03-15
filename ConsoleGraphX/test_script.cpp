#include "test_script.h"
#include "debugger.h"
#include "entity.h"
#include "script.h"

TestScript::TestScript() : Script()
{}

void TestScript::Awake(Entity* owner)
{
}

void TestScript::Update(Entity* owner)
{
	//Debugger::S_LogMessage("Second script running", Debugger::LogLevel::INFO);
}



