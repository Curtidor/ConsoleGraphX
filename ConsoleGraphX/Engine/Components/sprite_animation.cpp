#include "PCH_CGX.h"
#include "Engine/Components/sprite_animation.h"

namespace ConsoleGraphX
{
    SpriteAnimationFrame::SpriteAnimationFrame(ConsoleGraphX_Internal::ResourceIndex textureIndex, uint32_t durationMS)
        : m_textureIndex(textureIndex), m_durationMS(durationMS)
    {
    }

    SpriteAnimation::SpriteAnimation()
        : _m_owningEntity(MAX_SIZE_T), _m_frames(std::vector<SpriteAnimationFrame>()) {}


    SpriteAnimation::SpriteAnimation(std::vector<SpriteAnimationFrame> frames, size_t owningEntity)
        : _m_owningEntity(owningEntity), _m_frames(std::move(frames)) {}

    const SpriteAnimationFrame& SpriteAnimation::GetCurrentFrame() const
    {
        return _m_frames[_m_currentFrame];
    }

    size_t SpriteAnimation::GetOwningEntity() const
    {
        return _m_owningEntity;
    }

    void SpriteAnimation::NextFrame()
    {
        if (_m_currentFrame < _m_frames.size() - 1)
        {
            ++_m_currentFrame;
        }
        else if (m_looping)
        {
            _m_currentFrame = 0;
        }
        else
        {
            m_paused = true;
        }
    }

    size_t SpriteAnimation::TotalFrames()
    {
        return _m_frames.size();
    }

    void SpriteAnimation::Clone(SpriteAnimation* animation) const
    {
        *animation = *this;
    }
}
