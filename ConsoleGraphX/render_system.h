#pragma once
#include <vector>
#include <iostream>
#include "screen.h"
#include "sprite.h"
#include "entity.h"


class RenderSystem
{
public:
	static void DrawSprites(const std::vector<Entity*>& entities);

	static void DrawSprite_SS(const Vector3 position, Sprite* sprite);
	static void DrawSprite_SP(const Vector3 position, Sprite* sprite);

	static void DrawSprites_SS(const std::vector<Entity*>& entities);
	static void DrawSprites_SP(const std::vector<Entity*>& entities);

	static void DrawLine(Vector2 origin, Vector2 end, int color);
};