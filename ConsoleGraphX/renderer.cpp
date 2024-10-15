#include "CGXPCH.h"
#include <cmath>
#include "renderer.h"
#include "screen.h"
#include "sprite.h"
#include "resource_manager.h"
#include "resource_pool.h"
#include "camera.h"
#include "verify_macro.h"
#include "texture.h"
#include "transform.h"


/*
Parallelization: If you have a multi-core CPU, you could explore parallelizing the rendering process.
For example, you could divide the screen into smaller regions and assign a separate thread
to copy pixels for each region. This can improve performance, especially if you have a lot of sprites to render.
*/

namespace ConsoleGraphX_Internal 
{
    void Renderer::DrawSprites(float alpha)
    {
        ResourcePool<ConsoleGraphX::Camera>& cameraPool = ConsoleGraphX_Internal::ResourceManager::GetActiveResourceManager().GetResourcePool<ConsoleGraphX::Camera>();
        ResourcePool<ConsoleGraphX::Sprite>& spritePool = ConsoleGraphX_Internal::ResourceManager::GetActiveResourceManager().GetResourcePool<ConsoleGraphX::Sprite>();

        std::vector<ConsoleGraphX::Camera>* cameras = cameraPool.GetPoolItems();
        std::vector<ConsoleGraphX::Sprite>* sprites = spritePool.GetPoolItems();

        for (const ConsoleGraphX::Camera& cam : *cameras)
        {
            ConsoleGraphX::Vector3 cameraPosition = cam.GetPosition();
            cameraPosition.RoundD();

            ConsoleGraphX::Vector2 camViewPort = cam.GetViewPort();

            ConsoleGraphX::Vector3 relativePosition;
            for (const ConsoleGraphX::Sprite& sprite : *sprites)
            {
                // Get both the current and previous positions of the sprite
                const ConsoleGraphX::Transform* spriteTransform = sprite.GetTransform();
                ConsoleGraphX::Vector3 currentSpritePosition = spriteTransform->GetWorldPosition();
               // ConsoleGraphX::Vector3 previousSpritePosition = spriteTransform->GetPreviousPosition();

                // Interpolate between the previous and current positions using alpha
                currentSpritePosition.RoundD();
                //previousSpritePosition.RoundD();

                //ConsoleGraphX::Vector3 interpolatedSpritePosition = previousSpritePosition * (1.0f - alpha) + currentSpritePosition * alpha;

                // Calculate relative position to the camera
                relativePosition.x = currentSpritePosition.x - cameraPosition.x;
                relativePosition.y = currentSpritePosition.y - cameraPosition.y;

                // Check if the sprite is within the camera's view
                OverlapPoints overlapPoints;
                _CalculateEntityOverlapWithCamera(relativePosition, cameraPosition, camViewPort, sprite, overlapPoints);

                if (!_IsEntityVisibleInView(overlapPoints, sprite.Size()))
                    continue;

                // Draw the sprite with the interpolated position
                Renderer::_DrawSprite(relativePosition, sprite, overlapPoints);
            }
        }
    }


    void Renderer::_DrawSprite(const ConsoleGraphX::Vector3& relEntityPosition, const ConsoleGraphX::Sprite& sprite, const OverlapPoints& overlapPoints)
    {
        CHAR_INFO* buffer = Screen::GetActiveScreenBuffer_A();
        CHAR_INFO* pixels = ConsoleGraphX_Internal::ResourceManager::GetActiveResourceManager().GetResourcePool<Texture>().GetResourceFromPool(sprite.m_textureIndex)->GetPixels();

        CGX_VERIFY(pixels, "Null texture");

        const int spriteWidth = sprite.GetWidth();
        const int spriteHeight = sprite.GetHeight();

        const int screenWidth = Screen::GetWidth_A();

        int buffer_offset = static_cast<int>((overlapPoints.left > 0 ? 0 : relEntityPosition.x) + (overlapPoints.top > 0 ? 0 : relEntityPosition.y) * screenWidth);

        CHAR_INFO* pixelStartOffset = pixels + static_cast<int>(overlapPoints.left);
        CHAR_INFO* pixelEndOffset = pixels - static_cast<int>(overlapPoints.right);
        
        // overlap points are rounded so casting to int wont loss any data
        for (int y = static_cast<int>(overlapPoints.top); y < spriteHeight - overlapPoints.bottom; y++)
        {
            CHAR_INFO* srcStart = pixelStartOffset + (y * spriteWidth);
            CHAR_INFO* srcEnd = pixelEndOffset + ((y + 1) * spriteWidth);

            CHAR_INFO* dest = buffer + buffer_offset;

            Screen::SetPixels_A(srcStart, srcEnd, dest);

            buffer_offset += screenWidth;
        }
    }

};