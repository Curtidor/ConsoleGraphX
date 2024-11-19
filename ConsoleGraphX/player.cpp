#include "player.h"
#include "entity.h"
#include "player_controller.h"
#include "script.h"
#include "sprite.h"
#include "transform.h"
#include "input_system.h"
#include "camera.h"
#include "vector3.h"
#include "resource_manager.h"
#include "base_resource_pool.h"

using namespace ConsoleGraphX;

Player::Player() : Script(nullptr), _m_playerCam(nullptr)
{}

Player::Player(Entity* owner) : Script(owner), _m_playerCam(nullptr)
{}

Player::~Player()
{}

void Player::Awake()
{
	// TODO: look into what we want the behavoir to be should we expose the entites resource manager with a getter and use that manager
	// or use the scenes manager which may or may not be the same as the entites
	ConsoleGraphX_Internal::ResourceIndex index = ConsoleGraphX_Internal::ResourceManager::GetActiveResourceManager().CreateTextureResource("../player_sprite.cxsp").second;

	_m_playerCam = _m_owner->GetComponent<Camera>();

	_m_owner->AddComponent<Sprite>(index);

	Transform* transform = _m_owner->GetTransform();
	Sprite* sprite = _m_owner->GetComponent<Sprite>();

	_m_owner->GetComponent<Transform>()->SetPosition(50,50);
	_m_owner->AddComponent<PlayerController>(100, 5, 9);
}

void Player::Update(float deltaTime)
{
	Vector3 playerPosition = _m_owner->GetComponent<Transform>()->m_position;

	if (InputSystem::IsKeyPressed(Key::B))
	{
		_m_owner->KillEntity();

	}
}


