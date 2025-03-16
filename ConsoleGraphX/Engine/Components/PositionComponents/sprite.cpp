#include "PCH_CGX.h"
#include <cstdint>
#include "Engine\Components\PositionComponents\sprite.h"
#include "Engine\Components\PositionComponents\position_component.h"
#include "Engine\Components\transform.h"
#include "Engine\Resources\Pools\base_resource_pool.h"
#include "Engine\Graphics\texture.h"
#include "Engine\Resources\resource_manager.h"
#include "Engine\Systems\scene_system.h"

namespace ConsoleGraphX
{
	Sprite::Sprite()
		: PositionComponentBase(), _m_width(1), _m_height(1),
		m_textureIndex(-1), _m_isVisible(true), _m_isTransparent(false),
		m_layer(0), m_size(0, 0) 
	{}

	// used when creating a sprite from a already made texture
	Sprite::Sprite(ConsoleGraphX_Internal::ResourceIndex textureIndex, ConsoleGraphX_Internal::ResourceManager& rManager, TransformID transformID)
		: PositionComponentBase(transformID), _m_width(1), _m_height(1),
		m_textureIndex(textureIndex), _m_isVisible(true), _m_isTransparent(false),
		m_layer(0), m_size(0, 0) 
	{
		ConsoleGraphX_Internal::Texture* t = rManager.GetResource<ConsoleGraphX_Internal::Texture>(textureIndex);
		_m_width = t->GetWidth();
		_m_height = t->GetHeight();

		m_size = Vector2(_m_width, _m_height);
	}

	// used when creating a sprite with no texture
	Sprite::Sprite(uint32_t width, uint32_t height, int color, ConsoleGraphX_Internal::ResourceManager& rManager, TransformID transformD)
		: PositionComponentBase(transformD), _m_width(width), _m_height(height),
		m_textureIndex(-1), _m_isVisible(true), _m_isTransparent(false),
		m_layer(0), m_size(width, height)
	{
		m_textureIndex = rManager.CreateTextureResource(width, height, color).second;
	}

	// used when creating a sprite with no texture
	Sprite::Sprite(int width, int height, int color, ConsoleGraphX_Internal::ResourceManager& rManager, TransformID transformD)
		: PositionComponentBase(transformD), _m_width(width), _m_height(height),
		m_textureIndex(-1), _m_isVisible(true), _m_isTransparent(false),
		m_layer(0), m_size(width, height)
	{
		m_textureIndex = rManager.CreateTextureResource(width, height, color).second;
	}

	// used when cloning sprites
	Sprite::Sprite(TransformID transformID)
		: PositionComponentBase(transformID), _m_width(1), _m_height(1),
		m_textureIndex(-1), _m_isVisible(true), _m_isTransparent(false),
		m_layer(0), m_size(0, 0)
	{}

	Sprite::Sprite(const Sprite& other)
		: PositionComponentBase(other.m_transformID), _m_width(other._m_width), _m_height(other._m_height),
		_m_isVisible(other._m_isVisible), _m_isTransparent(other._m_isTransparent),
		m_layer(other.m_layer), m_size(other.m_size), m_textureIndex(other.m_textureIndex)
	{}

	void Sprite::Clone(Sprite* sprite) const
	{
		*sprite = *this;
	}

	Sprite& Sprite::operator=(const Sprite& other) 
	{
		if (this == &other) 
		{
			return *this;
		}

		_m_width = other._m_width;
		_m_height = other._m_height;
		_m_isVisible = other._m_isVisible;
		_m_isTransparent = other._m_isTransparent;
		m_textureIndex = other.m_textureIndex;

		m_layer = other.m_layer;
		m_size = other.m_size;

		return *this;
	}

	const Vector2& Sprite::Size() const
	{
		return m_size;
	}

	void Sprite::HideSprite() { _m_isVisible = false; }
	void Sprite::ShowSprite() { _m_isVisible = true; }

	bool Sprite::IsSpriteHidden() const { return _m_isVisible; }
	bool Sprite::IsTransparent() const { return _m_isTransparent; }

	int Sprite::GetHeight() const { return _m_height; }
	int Sprite::GetWidth() const { return _m_width; }


};