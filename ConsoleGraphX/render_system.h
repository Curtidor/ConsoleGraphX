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

	/**
	 * @brief Checks if an entity is visible in the camera's view.
	 * @param overlapPoints The overlap points of the entity's sprite with the camera's view.
	 * @param spriteSize The size of the sprite.
	 * @return True if the entity is visible in the camera's view, false otherwise.
	 */
	static bool _IsEntityVisibleInView(const _OverlapPoints& overlapPoints, const Vector2& spriteSize);

	/**
	 * @brief Calculates the overlap of the entity's sprite with the camera's view.
	 * @param entityPosition The position of the entity.
	 * @param camPosition The position of the camera.
	 * @param viewPortSize The size of the viewport.
	 * @param sprite Pointer to the sprite component of the entity.
	 * @param overlapPoints Reference to store the calculated overlap points.
	 */
	static void _CalculateEntityOverlapWithCamera(const Vector3& entityPosition, const Vector3& camPosition, const Vector2& viewPortSize, Sprite* sprite, _OverlapPoints& overlapPoints);
	/**
	 * @brief Draws a sprite onto the screen buffer within the camera's view.
	 * @param relEntityPosition The relative position of the entity within the camera's view.
	 * @param sprite Pointer to the sprite to be drawn.
	 * @param overlapPoints The overlap points of the sprite with the camera's view.
	 */
	static void _DrawSprite_SS(const Vector3& relEntityPosition, Sprite* sprite, const _OverlapPoints& overlapPoints);
	static void _DrawSprite_SP(const Vector3& relEntityPosition, Sprite* sprite, const _OverlapPoints& overlapPoints);

public:
	/**
	 * @brief Draws sprites of entities within the camera's view.
	 * @param entities Vector of pointers to entities to be drawn.
	 */
	static void DrawSprites(const std::vector<Entity*>& entities);

	static void DrawLine(Vector2 origin, Vector2 end, int color);
};