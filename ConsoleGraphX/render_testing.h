#pragma once
#include <functional>
#include "entity.h"
#include <string>
#include <vector>
#include "sprite.h"
#include "vector3.h"
#include "camera.h"

class RenderTest
{
public:
	static void Run();

	static int GetRandomNumberInRange(int lower_bound, int upper_bound);
	static void CreateRandomEntitys(std::vector<Entity*>* entities, int amount, int width, int height);

	static void TimeSpriteDraw(std::function<void(const Vector3, Sprite*, Camera*)> drawFunction, const Vector3 vec, Sprite* sprite, Camera* cam, int n, const std::string& message);
	static void TimeSpritesDraw(std::function<void(const std::vector<Entity*>&)> drawFunction, const std::vector<Entity*>& entities, int n, const std::string& message);
};