#pragma once
#include <unordered_map>
#include <typeindex>
#include <type_traits>
#include "Engine\Systems\base_system.h"
#include "Engine\Systems\scene_system.h"

namespace ConsoleGraphX_Internal
{
	/**
	 * @brief Manages systems using a runtime type index.
	 *
	 * This class stores systems in an unordered_map using std::type_index.
	 * Unlike components, system lookups are infrequent, so we do not use a
	 * compile-time ID system for performance. The overhead of std::type_index
	 * is negligible given that system queries are rare.
	 */
	enum CALLTYPE
	{
		EndOfFrame = 1
	};

	class SystemManager
	{
	private:
		std::unordered_map<std::type_index, ConsoleGraphX::BaseSystem*> _m_systems;
		std::unordered_map<CALLTYPE, std::vector<ConsoleGraphX::BaseSystem*>> _m_callTypeSystems;

	public:
		~SystemManager();

		template <typename SystemType>
		void RegisterSystem()
		{
			static_assert(std::is_base_of<ConsoleGraphX::BaseSystem, SystemType>::value,
				"The passed type must be derived from BaseSystem.");

			std::type_index typeIndex(typeid(SystemType));

			// prevent duplicate registration
			if (_m_systems.find(typeIndex) != _m_systems.end())
				return;

			ConsoleGraphX::BaseSystem* system = new SystemType();
			system->Initialize();
			_m_systems[typeIndex] = system;
		}

		template <typename SystemType>
		SystemType* GetSystem()
		{
			static_assert(std::is_base_of<ConsoleGraphX::BaseSystem, SystemType>::value,
				"The requested type must be derived from BaseSystem.");

			auto it = _m_systems.find(std::type_index(typeid(SystemType)));
			if (it != _m_systems.end())
				return static_cast<SystemType*>(it->second);

			return nullptr; // System not found
		}

		void RegisterCallTypeSystem(CALLTYPE callType, ConsoleGraphX::BaseSystem* system);
		void CallSystems(CALLTYPE callType);

		void Update(float deltaTime, ConsoleGraphX::SceneSystem& sceneSystem);
		void ShutDown();
	};
};

