#pragma once
#include "Engine/Components/component.h"
#include <Engine/Resources/Pools/base_resource_pool.h>
#include <vector>

namespace ConsoleGraphX
{
    struct SpriteAnimationFrame
    {
        SpriteAnimationFrame(ConsoleGraphX_Internal::ResourceIndex textureIndex, uint32_t durationMS);

        ConsoleGraphX_Internal::ResourceIndex m_textureIndex = (std::numeric_limits<size_t>::max)();
        uint32_t m_durationMS = (std::numeric_limits<uint32_t>::max)();
    };

    struct SpriteAnimation : public ConsoleGraphX_Internal::Component
    {
    private:
        uint16_t _m_currentFrame = 0;
        std::vector<SpriteAnimationFrame> _m_frames;
        size_t _m_owningEntity;

    public:
        bool m_paused = false;
        bool m_looping = false;
        float m_timeAccumulator = 0.0f;

        SpriteAnimation();
        SpriteAnimation(std::vector<SpriteAnimationFrame> frames, size_t owningEntity);

        const SpriteAnimationFrame& GetCurrentFrame() const;
        size_t GetOwningEntity() const;
        size_t TotalFrames();
        void NextFrame();
        void Clone(SpriteAnimation* animation) const;
    };
}
