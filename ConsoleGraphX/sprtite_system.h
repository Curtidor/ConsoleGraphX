#pragma once
#include <set>
#include "base_system.h"
#include "entity.h"
#include <vector>

namespace ConsoleGraphX_Internal
{
	struct SpriteComparator {
		bool operator()(const ConsoleGraphX::Entity* entityA, const ConsoleGraphX::Entity* entityB) const;
	};

	class SpriteSystem : public ConsoleGraphX::BaseSystem
	{
	private:
		static std::multiset<ConsoleGraphX::Entity*, SpriteComparator> _s_entitySprites;

	public:
		void Initialize() override;
		void Update(float delta_time) override;

		static void RegisterEntitySprite(ConsoleGraphX::Entity* entity);

		static void DeregisterEntitySprite(ConsoleGraphX::Entity* entity);

		static std::vector<ConsoleGraphX::Entity*> GetEntitySprites();

	};
};


