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


bool RenderSystem::_IsEntityNotInCamView(const _OverlapPoints& overlapPoints, const Vector2& spriteSize)
{
    return (overlapPoints.left >= spriteSize.x ||
        overlapPoints.right >= spriteSize.x ||
        overlapPoints.top >= spriteSize.y ||
        overlapPoints.bottom >= spriteSize.y);
}



void RenderSystem::_CalculateEntityOverLapRelCamera(const Vector3& relEntityPosition, Camera* cam, Sprite* sprite, _OverlapPoints& overlapPoints)
{
    const Vector3 camPosition = cam->GetPosition();

    const int spriteWidth = sprite->GetWidth();
    const int spriteHeight = sprite->GetHeight();

    const int renderWidth = cam->GetWidth();
    const int renderHeight = cam->GetHeight();

    const int camX = std::floorf(camPosition.x);
    const int camY = std::floorf(camPosition.y);

    const int entityX = std::floorf(relEntityPosition.x); 
    const int entityY = std::floorf(relEntityPosition.y);  

    overlapPoints.left = std::max<int>(0, camX - entityX);
    overlapPoints.right = std::max<int>(0, ((overlapPoints.left > 0 ? 0 : entityX) + spriteWidth - overlapPoints.left) - renderWidth);

    overlapPoints.top = std::max<int>(0, camY - entityY);
    overlapPoints.bottom = std::max<int>(0, ((overlapPoints.top > 0 ? 0 : entityY) + spriteHeight - overlapPoints.top) - renderHeight);
}


void RenderSystem::DrawSprites(const std::vector<Entity*>& entities)
{
    std::unordered_set<Camera*> activeCams = CameraSystem::GetActiveCameras();

    if (activeCams.size() == 0)
        throw std::runtime_error("No active cameras found");


    for (Camera* cam : activeCams)
    {
        Vector3 cameraPosition = cam->GetPosition();

        for (Entity* entity : entities)
        {
            Sprite* sprite = entity->GetComponent<Sprite>();

            if (sprite == nullptr)
            {
                Debugger::S_LogMessage("entity does not have the sprite component DRAW SPRITES", Debugger::LogLevel::WARNING);
                continue;
            }

            Vector3 entityPosition = entity->GetPosition();

            Vector3 relativePosition = {entityPosition.x - cameraPosition.x, entityPosition.y + cameraPosition.y, 0};

            _OverlapPoints overlapPoints;
            _CalculateEntityOverLapRelCamera(relativePosition, cam, sprite, overlapPoints);

            if (_IsEntityNotInCamView(overlapPoints, sprite->Size()))
                continue;

            RenderSystem::DrawSprite_SS(relativePosition, sprite, overlapPoints);
        }
    }
}

void RenderSystem::DrawSprite_SS(const Vector3& relEntityPosition, Sprite* sprite, const _OverlapPoints& overlapPoints)
{
    if (!sprite)
       throw std::runtime_error("[RENDER SYSTEM], null sprite pointer");

    CHAR_INFO* buffer = Screen::GetActiveScreenBuffer_A();
    CHAR_INFO* pixels = sprite->GetPixels();

    const int spriteWidth = sprite->GetWidth();
    const int spriteHeight = sprite->GetHeight();

    const int screenWidth = Screen::GetWidth_A();

    const int entityX = std::floorf(relEntityPosition.x);
    const int entityY = std::floorf(relEntityPosition.y);

    // if the entity is off the left side of the screen use 0 for the x position as the first visible part of sprite starts at x 0
    int buffer_offset = (overlapPoints.left > 0 ? 0 : entityX) + entityY * screenWidth;

    for (int y = overlapPoints.top; y < spriteHeight-overlapPoints.bottom; y++)
    {
        CHAR_INFO* srcStart = pixels + (y * spriteWidth) + overlapPoints.left;
        CHAR_INFO* srcEnd = pixels + (((y + 1) * spriteWidth) - overlapPoints.right);

        //Debugger::S_LogMessage("SORUCE LEGTH: " + std::to_string(src_end - src_start));

        CHAR_INFO* dest = buffer + buffer_offset;

        Screen::SetPixels_A(srcStart, srcEnd, dest);

        buffer_offset += screenWidth;
    }
}


void RenderSystem::DrawSprite_SP(const Vector3& relEntityPosition, Sprite* sprite, const _OverlapPoints& overlapPoints)
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

void RenderSystem::DrawSprites_SS(const std::vector<Entity*>& entities)
{
    std::unordered_set<Camera*> activeCams = CameraSystem::GetActiveCameras();

    if (activeCams.size() == 0)
        throw std::runtime_error("No active cameras found");

    for (Camera* cam : activeCams) {

        Vector3 cameraPosition = cam->GetPosition();

        for (Entity* entity : entities)
        {
            Sprite* sprite = entity->GetComponent<Sprite>();
            
            Vector3 entityPosition = entity->GetPosition();

            Vector3 relativePosition = { entityPosition.x - cameraPosition.x, entityPosition.y + cameraPosition.y, 0 };

            _OverlapPoints overlapPoints;
            _CalculateEntityOverLapRelCamera(relativePosition, cam, sprite, overlapPoints);

            if (!_IsEntityNotInCamView(overlapPoints, sprite->Size()))
                continue;

            RenderSystem::DrawSprite_SS(relativePosition, sprite, overlapPoints);
        }
    }
   
}

void RenderSystem::DrawSprites_SP(const std::vector<Entity*>& entities)
{
    std::unordered_set<Camera*> activeCams = CameraSystem::GetActiveCameras();

    if (activeCams.size() == 0)
        throw std::runtime_error("No active cameras found");

    for (Camera* cam : activeCams) {
        for (Entity* entity : entities)
        {
            Sprite* sprite = entity->GetComponent<Sprite>();
            
            const Vector3 entityPosition = entity->GetPosition();
            const Vector3 camPosition = cam->GetPosition();

            _OverlapPoints overlapPoints;
            const Vector3 relativePosition = entityPosition + camPosition;

            RenderSystem::DrawSprite_SP(relativePosition, sprite, overlapPoints);
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



