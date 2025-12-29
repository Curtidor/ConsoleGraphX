#include "PCH_CGX.h"
#include <cmath>
#include <limits>
#include "Engine\Graphics\renderer.h"
#include "Engine\Graphics\ScreenGraphics\screen.h"
#include "Engine\Components\PositionComponents\sprite.h"
#include "Engine\Resources\resource_manager.h"
#include "Engine\Resources\Pools\resource_pool.h"
#include "Engine\Components\camera.h"
#include "Engine\Core\Utils\verify_macro.h"
#include "Engine\Graphics\texture.h"
#include "Engine\Components\transform.h"
#include "Engine\Systems\scene_system.h"
#include "Engine\Core\Profiler\profiler.h"


/*
Parallelization: If you have a multi-core CPU, you could explore parallelizing the rendering process.
For example, you could divide the screen into smaller regions and assign a separate thread
to copy pixels for each region. This can improve performance, especially if you have a lot of sprites to render.
*/

namespace ConsoleGraphX_Internal 
{
    const void Renderer::DrawSprites(const Screen& screen, ConsoleGraphX::SceneSystem& sceneSystem, float alpha)
    {
        PROFILE_SCOPE("RENDER");

        ResourceManager& activeManager = sceneSystem.GetActiveResourceManager();

        ResourcePool<ConsoleGraphX::Camera>& cameraPool = activeManager.GetResourcePool<ConsoleGraphX::Camera>();
        ResourcePool<ConsoleGraphX::Sprite>& spritePool = activeManager.GetResourcePool<ConsoleGraphX::Sprite>();

        std::vector<ConsoleGraphX::Camera>* cameras = cameraPool.GetPoolItems();
        std::vector<ConsoleGraphX::Sprite>* sprites = spritePool.GetPoolItems();

        INCREMENT_COUNTER("TOTAL SPRITES", sprites->size());

        for (const ConsoleGraphX::Camera& cam : *cameras)
        {
            ConsoleGraphX::Vector3 cameraPosition = cam.GetPosition();
            cameraPosition.RoundD();

            ConsoleGraphX::Vector2i camViewPort = cam.GetViewPort();
            camViewPort.x = std::min<int32_t>(camViewPort.x, screen.GetWidth());
            camViewPort.y = std::min<int32_t>(camViewPort.y, screen.GetHeight());

            ConsoleGraphX::Vector3 relativePosition;
            for (const ConsoleGraphX::Sprite& sprite : *sprites)
            {

                if (sprite.m_textureIndex == (std::numeric_limits<ConsoleGraphX::TransformID>::max)()) // the sprite is recycled
                {
                    continue;
                }

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

                INCREMENT_COUNTER("ON SCREEN SPRITES", 1);

                // Draw the sprite with the interpolated position
                Renderer::_DrawSprite(screen, sceneSystem, relativePosition, sprite, overlapPoints);
            }
        }
    }


    const void Renderer::_DrawSprite(const Screen& screen, ConsoleGraphX::SceneSystem& sceneSystem, const ConsoleGraphX::Vector3& relEntityPosition, const ConsoleGraphX::Sprite& sprite, const OverlapPoints& overlapPoints)
    {
        CHAR_INFO* buffer = Screen::GetActiveScreenBuffer_A();
        CHAR_INFO* pixels = sceneSystem.GetActiveResourceManager().GetResourcePool<Texture>().GetResourceFromPool(sprite.m_textureIndex)->GetPixels();

        CGX_VERIFY(pixels, "Null texture");

        const int spriteWidth = sprite.GetWidth();
        const int spriteHeight = sprite.GetHeight();

        const int screenWidth = screen.GetWidth();

        int buffer_offset = static_cast<int>((overlapPoints.left > 0 ? 0 : relEntityPosition.x) + (overlapPoints.top > 0 ? 0 : relEntityPosition.y) * screenWidth);

        CHAR_INFO* pixelStartOffset = pixels + static_cast<int>(overlapPoints.left);
        CHAR_INFO* pixelEndOffset = pixels - static_cast<int>(overlapPoints.right);
        
        // overlap points are rounded so casting to int wont loss any data
        for (int y = static_cast<int>(overlapPoints.top); y < spriteHeight - overlapPoints.bottom; y++)
        {
            CHAR_INFO* srcStart = pixelStartOffset + (y * spriteWidth);
            CHAR_INFO* srcEnd = pixelEndOffset + ((y + 1) * spriteWidth);

            CHAR_INFO* dest = buffer + buffer_offset;

            screen.SetPixels(srcStart, srcEnd, dest);

            buffer_offset += screenWidth;
        }
    }

};