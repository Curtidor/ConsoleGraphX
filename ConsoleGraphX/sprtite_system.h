#pragma once
#include <set>
#include "system.h"
#include "entity.h"
#include <vector>

namespace ConsoleGraphX_Interal
{
	struct SpriteComparator {
		bool operator()(const ConsoleGraphX::Entity* entityA, const ConsoleGraphX::Entity* entityB) const;
	};

	class SpriteSystem : public ConsoleGraphX::System
	{
	private:
		static std::multiset<ConsoleGraphX::Entity*, SpriteComparator> _s_entitySprites;

	public:
		void Initialize() const override;
		void Update(float delta_time) const override;

		static void RegisterEntitySprite(ConsoleGraphX::Entity* entity);

		static void DeregisterEntitySprite(ConsoleGraphX::Entity* entity);

		static std::vector<ConsoleGraphX::Entity*> GetEntitySprites();

	};
};


