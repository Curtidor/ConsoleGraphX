#include <stdexcept>
#include "script.h"
#include "entity.h"
#include "verify_macro.h"

namespace ConsoleGraphX
{
	Script::Script() : _m_isEnabled(true), _m_owner(nullptr)
	{}

	Script::Script(Entity* owner) : _m_isEnabled(true), _m_owner(owner)
	{
		CGX_VERIFY(owner);
	}

	Script::Script(const Script& script, Entity* owner) : _m_isEnabled(script._m_isEnabled), _m_owner(owner)
	{}

	void Script::Clone(Script*& script)
	{
		script = new Script(*this, script->_m_owner);
	}

	void Script::Clone(Script*& script, Entity* owner)
	{
		script = new Script(*this, owner);
	}

	void Script::SetOwner(Entity* owner)
	{
		if (owner != nullptr && _m_owner == nullptr)
		{
			_m_owner = owner;
		}
		else if (_m_owner != nullptr)
		{
			throw std::runtime_error("cannot reassign owning entity");
		}
	}

	const Entity* Script::GetOwner() const
	{
		return _m_owner;
	}

	void Script::Awake() {}
	void Script::Start() {}
	void Script::Update(float deltaTime) {}

	bool Script::IsEnabled() const
	{
		return _m_isEnabled;
	}

	void Script::SetState(bool state)
	{
		_m_isEnabled = state;
	}

};
