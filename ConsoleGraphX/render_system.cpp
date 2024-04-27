#include "render_system.h"
#include <cmath>
#include <utility>
#include <vector>
#include <windows.h>
#include <stdexcept>
#include <unordered_set>
#include "debugger.h"
#include "entity.h"
#include "screen.h"
#include "sprite.h"
#include "vector2.h"
#include "vector3.h"
#include "camera_system.h"
#include "camera.h"
#include "transform.h"


/*
Parallelization: If you have a multi-core CPU, you could explore parallelizing the rendering process.
For example, you could divide the screen into smaller regions and assign a separate thread 
to copy pixels for each region. This can improve performance, especially if you have a lot of sprites to render.
*/


bool RenderSystem::_IsEntityVisibleInView(const _OverlapPoints& overlapPoints, const Vector2& spriteSize)
{
    return !(overlapPoints.left >= spriteSize.x ||
        overlapPoints.right >= spriteSize.x ||
        overlapPoints.top >= spriteSize.y ||
        overlapPoints.bottom >= spriteSize.y);
}

void RenderSystem::_CalculateEntityOverlapWithCamera(const Vector3& entityPosition, const Vector3& camPosition, const Vector2& viewPortSize, Sprite* sprite, _OverlapPoints& overlapPoints)
{
    const int spriteWidth = sprite->GetWidth();
    const int spriteHeight = sprite->GetHeight();

    overlapPoints.left = std::abs(std::min<int>(entityPosition.x - camPosition.x, 0));
    overlapPoints.right = std::max<int>(0, ((overlapPoints.left > 0 ? 0 : entityPosition.x - camPosition.x) + spriteWidth - overlapPoints.left) - viewPortSize.x);

    overlapPoints.top = std::abs(std::min<int>(entityPosition.y - camPosition.y, 0));
    overlapPoints.bottom = std::max<int>(0, ((overlapPoints.top > 0 ? 0 : entityPosition.y - camPosition.y) + spriteHeight - overlapPoints.top) - viewPortSize.y);
}

void RenderSystem::DrawSprites(const std::vector<Entity*>& entities)
{
    std::unordered_set<Camera*> activeCams = CameraSystem::GetActiveCameras();

    for (Camera* cam : activeCams)
    {
        Vector3 cameraPosition = cam->GetPosition();

        cameraPosition.x = std::floorf(cameraPosition.x);
        cameraPosition.y = std::floorf(cameraPosition.y);

        Vector2 viewPortSize = Vector2{ static_cast<float>(cam->GetWidth()),static_cast<float>(cam->GetHeight()) };

        for (Entity* entity : entities)
        {
            Vector3 entityPosition = entity->GetPosition();
            entityPosition.x = std::floorf(entityPosition.x);
            entityPosition.y = std::floorf(entityPosition.y);

            // we don't check the sprite pointer as all entities in the sprite system are guaranteed to have a sprite component
            Sprite* sprite = entity->GetComponent<Sprite>();

            _OverlapPoints overlapPoints;
            _CalculateEntityOverlapWithCamera(entityPosition, cameraPosition, viewPortSize, sprite, overlapPoints);

            if (!_IsEntityVisibleInView(overlapPoints, sprite->Size()))
                continue;

            Vector3 relativePosition = { entityPosition.x - cameraPosition.x, entityPosition.y - cameraPosition.y };

            RenderSystem::_DrawSprite_SS(relativePosition, sprite, overlapPoints);
        }
    }
}

void RenderSystem::_DrawSprite_SS(const Vector3& relEntityPosition, Sprite* sprite, const _OverlapPoints& overlapPoints)
{
    if (!sprite)
       throw std::runtime_error("[RENDER SYSTEM], null sprite pointer");

    CHAR_INFO* buffer = Screen::GetActiveScreenBuffer_A();
    CHAR_INFO* pixels = sprite->GetPixels();

    const int spriteWidth = sprite->GetWidth();
    const int spriteHeight = sprite->GetHeight();

    const int screenWidth = Screen::GetWidth_A();

    int buffer_offset = (overlapPoints.left > 0 ? 0 : relEntityPosition.x) + (overlapPoints.top > 0 ? 0 : relEntityPosition.y) * screenWidth;

    CHAR_INFO* pixelStartOffset = pixels + overlapPoints.left;
    CHAR_INFO* pixelEndOffset = pixels - overlapPoints.right;

    for (int y = overlapPoints.top; y < spriteHeight-overlapPoints.bottom; y++)
    {
        CHAR_INFO* srcStart = pixelStartOffset + (y * spriteWidth);
        CHAR_INFO* srcEnd = pixelEndOffset + ((y + 1) * spriteWidth);

        CHAR_INFO* dest = buffer + buffer_offset;

        Screen::SetPixels_A(srcStart, srcEnd, dest);

        buffer_offset += screenWidth;
    }
}


void RenderSystem::_DrawSprite_SP(const Vector3& relEntityPosition, Sprite* sprite, const _OverlapPoints& overlapPoints)
{
    if (!sprite)
        return;
  
    CHAR_INFO* pixels = sprite->GetPixels();

    const int spriteWidth = sprite->GetWidth();
    const int spriteHeight = sprite->GetHeight();

    for (int y = 0; y < spriteHeight; y++)
    {
        for (int x = 0; x < spriteWidth; x++)
        {
            Screen::SetPixel_A(relEntityPosition.x + x, relEntityPosition.y + y, *pixels++);
        }
    }
}

void RenderSystem::DrawLine(Vector2 origin, Vector2 end, int color)
{
    CHAR_INFO s_pixel{ Screen::s_pixel, color };

    int dx = end.x - origin.x;
    int dy = end.y - origin.y;

    int steps = abs(dx) > abs(dy) ? abs(dx) : abs(dy);

    float xIncrement = static_cast<float>(dx) / steps;
    float yIncrement = static_cast<float>(dy) / steps;

    float x = origin.x;
    float y = origin.y;

    for (int i = 0; i <= steps; i++) {
        int ix = static_cast<int>(x);
        int iy = static_cast<int>(y);
        
        Screen::SetPixel_A(ix, iy, s_pixel);
        
        x += xIncrement;
        y += yIncrement;
    }
}



