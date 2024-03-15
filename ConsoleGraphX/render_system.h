#pragma once
#include <vector>
#include "sprite.h"
#include "entity.h"
#include "vector2.h"
#include "vector3.h"
#include "camera.h"


class RenderSystem
{
private:
	struct _OverlapPoints
	{
		int left;
		int right;
		int top;
		int bottom;
	};

	static bool _IsEntityNotInCamView(const _OverlapPoints& overlapPoints , const Vector2& spriteSize);
	static void _CalculateEntityOverLapRelCamera(const Vector3& relEntityPosition, Camera* cam, Sprite* sprite, _OverlapPoints& overlapPoints);

public:
	static void DrawSprites(const std::vector<Entity*>& entities);

	static void DrawSprite_SS(const Vector3& relEntityPosition, Sprite* sprite, const _OverlapPoints& overlapPoints);
	static void DrawSprite_SP(const Vector3& relEntityPosition, Sprite* sprite, const _OverlapPoints& overlapPoints);

	static void DrawSprites_SS(const std::vector<Entity*>& entities);
	static void DrawSprites_SP(const std::vector<Entity*>& entities);

	static void DrawLine(Vector2 origin, Vector2 end, int color);
};