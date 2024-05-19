#pragma once
#include <unordered_set>
#include <type_traits>
#include "base_system.h"

namespace ConsoleGraphX_Interal
{
	class SystemManager
	{
	private:
		std::unordered_set<ConsoleGraphX::BaseSystem*> _m_systems;

	public:
		~SystemManager();

		template <typename SystemType>
		void RegisterSystem()
		{
			static_assert(std::is_base_of<ConsoleGraphX::BaseSystem, SystemType>::value, "The passed type must be derived from BaseSystem.");

			ConsoleGraphX::BaseSystem* system = new SystemType();

			system->Initialize();

			_m_systems.insert(system);

		}

		void Update(float deltaTime);
	};
};