#include "PCH_CGX.h"
#include "sprite_animation_system.h"
#include "Engine\Systems\scene_system.h"

void ConsoleGraphX::SpriteAnimationSystem::Initialize()
{
}

void ConsoleGraphX::SpriteAnimationSystem::Update(float delta_time, SceneSystem& sceneSystem)
{
	ConsoleGraphX_Internal::ResourceManager& activeResourceManager = sceneSystem.GetActiveResourceManager();

	ConsoleGraphX_Internal::ResourcePool<SpriteAnimation>& animationPool = activeResourceManager.GetResourcePool<SpriteAnimation>();
	Scene* activeScene = sceneSystem.GetActiveScene();

	for(SpriteAnimation& animation : *animationPool.GetPoolItems())
	{
		if (animation.m_paused || animation.TotalFrames() == 0)
			continue;

        animation.m_timeAccumulator += delta_time * 1000;

        const auto& currentFrame = animation.GetCurrentFrame();

        if (animation.m_timeAccumulator >= static_cast<float>(currentFrame.m_durationMS))
        {
            animation.NextFrame();

			if (Entity* owner = activeScene->GetEntity(animation.GetOwningEntity());
				Sprite* s = owner->GetComponent<Sprite>())
			{
				s->m_textureIndex = animation.GetCurrentFrame().m_textureIndex;
				animation.m_timeAccumulator = 0.0f;
			}

        }
	}
}
