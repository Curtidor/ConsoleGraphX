#pragma once
#include <vector>
#include <utility>
#include <cmath>
#include "sprite.h"
#include "entity.h"
#include "vector2.h"
#include "vector3.h"

namespace ConsoleGraphX_Internal 
{
	class RenderSystem
	{
	private:
		struct OverlapPoints
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
		static inline bool _IsEntityVisibleInView(const OverlapPoints& overlapPoints, const ConsoleGraphX::Vector2& spriteSize)
		{
			return !(overlapPoints.left >= spriteSize.x ||
				overlapPoints.right >= spriteSize.x ||
				overlapPoints.top >= spriteSize.y ||
				overlapPoints.bottom >= spriteSize.y);
		}

		/**
		 * @brief Calculates the overlap of the entity's sprite with the camera's view.
		 * @param entityPosition The position of the entity.
		 * @param camPosition The position of the camera.
		 * @param viewPortSize The size of the viewport.
		 * @param sprite Pointer to the sprite component of the entity.
		 * @param overlapPoints Reference to store the calculated overlap points.
		 */
		static inline void _CalculateEntityOverlapWithCamera(const ConsoleGraphX::Vector3& entityPosition, const ConsoleGraphX::Vector3& camPosition, const ConsoleGraphX::Vector2& viewPortSize, ConsoleGraphX::Sprite* sprite, OverlapPoints& overlapPoints)
		{
			const int spriteWidth = sprite->GetWidth();
			const int spriteHeight = sprite->GetHeight();

			overlapPoints.left = std::abs(std::min<int>(entityPosition.x - camPosition.x, 0));
			overlapPoints.right = std::max<int>(0, ((overlapPoints.left > 0 ? 0 : entityPosition.x - camPosition.x) + spriteWidth - overlapPoints.left) - viewPortSize.x);

			overlapPoints.top = std::abs(std::min<int>(entityPosition.y - camPosition.y, 0));
			overlapPoints.bottom = std::max<int>(0, ((overlapPoints.top > 0 ? 0 : entityPosition.y - camPosition.y) + spriteHeight - overlapPoints.top) - viewPortSize.y);
		}
		/**
		 * @brief Draws a sprite onto the screen buffer within the camera's view.
		 * @param relEntityPosition The relative position of the entity within the camera's view.
		 * @param sprite Pointer to the sprite to be drawn.
		 * @param overlapPoints The overlap points of the sprite with the camera's view.
		 */
		static void _DrawSprite_SS(const ConsoleGraphX::Vector3& relEntityPosition, ConsoleGraphX::Sprite* sprite, const OverlapPoints& overlapPoints);

	public:
		/**
		 * @brief Draws sprites of entities within the camera's view.
		 * @param entities Vector of pointers to entities to be drawn.
		 */
		static void DrawSprites();

		static void DrawLine(ConsoleGraphX::Vector2 origin, ConsoleGraphX::Vector2 end, int color);
	};
};
