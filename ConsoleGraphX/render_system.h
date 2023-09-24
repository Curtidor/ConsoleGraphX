#pragma once
#include <vector>
#include "screen.h"
#include "sprite.h"
#include "entity.h"

class RenderSystem
{
public:
	static void DrawSprite_SS(Entity* entity);
	static void DrawSprite_SP(Entity* entity);

	static void DrawSprites_SS(const std::vector<Entity*>& entities);
	static void DrawSprites_SP(const std::vector<Entity*>& entities);

	static void DrawLine(Vector2 origin, Vector2 end, int color);
};