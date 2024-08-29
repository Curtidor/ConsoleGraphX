#include "script.h"
#include "component.h"
#include "entity.h"

namespace ConsoleGraphX
{
	Script::Script() : _m_isEnabled(true)
	{}

	Script::Script(const Script& script) : _m_isEnabled(true)
	{}

	void Script::Clone(Script*& script) const
	{
		script = new Script(*this);
	}

	void Script::Awake(Entity* owner) {}
	void Script::Start(Entity* owner) {}
	void Script::Update(Entity* owner) {}

	bool Script::IsEnabled() const
	{
		return _m_isEnabled;
	}

	void Script::SetState(bool state)
	{
		_m_isEnabled = state;
	}

};
