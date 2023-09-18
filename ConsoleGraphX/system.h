#pragma once
#include <unordered_set>

class System
{
private:
	static std::unordered_set<System*> _s_systems;

public:
	~System();

	template <typename SystemType>
	static void RegisterSystem()
	{
		static_assert(std::is_base_of<System, SystemType>::value, "The passed type must be derived from Component.");

		System* system = new SystemType();

		system->Initalize();

		_s_systems.insert(system);

	}
	virtual void Initalize() const;
	virtual void Update() const;
};