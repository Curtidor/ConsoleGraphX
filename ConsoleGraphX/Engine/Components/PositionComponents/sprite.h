#pragma once
#include <cstdint>
#include "Engine\Components\PositionComponents\position_component.h"
#include "Engine\Components\transform.h"
#include "Engine\Math\vector2.h"
#include "Engine\Components\component.h"
#include "Engine\Resources\Pools\base_resource_pool.h"

namespace ConsoleGraphX_Internal
{
    class ResourceManager;
}

namespace ConsoleGraphX
{
    struct Sprite : public ConsoleGraphX_Internal::PositionComponentBase, ConsoleGraphX_Internal::Component
    {
    private:
        uint32_t _m_width;
        uint32_t _m_height;
        bool _m_isVisible;
        bool _m_isTransparent;

    public:
        int m_layer;
        Vector2 m_size;
        ConsoleGraphX_Internal::ResourceIndex m_textureIndex;


    public:
        Sprite();
        // this should only be used by internal functions
        Sprite(TransformID transform); 
        Sprite(ConsoleGraphX_Internal::ResourceIndex textureIndex, ConsoleGraphX_Internal::ResourceManager* rManager, TransformID transform = -1);
        Sprite(uint32_t width, uint32_t height, int color, ConsoleGraphX_Internal::ResourceManager* rManager, TransformID transform = -1);
        Sprite(int width, int height, int color, ConsoleGraphX_Internal::ResourceManager* rManager, TransformID transform = -1);
        
        Sprite(const Sprite& other);

        void Clone(Sprite* sprite) const;

        Sprite& operator=(const Sprite& other);

        void HideSprite();
        void ShowSprite();

        bool IsSpriteHidden() const;
        bool IsTransparent() const;

        int GetWidth() const;
        int GetHeight() const;

        const Vector2& Size() const;
    };
};

