//#include "render_testing.h"
//#include <chrono>
//#include <functional>
//#include <iostream>
//#include <random>
//#include <string>
//#include <vector>
//#include "entity.h"
//#include "render_system.h"
//#include "screen.h"
//#include "sprite.h"
//#include "vector3.h"
//#include "camera.h"
//
///*
//* sprite size (0,0) => SS
//* sprite size (50, 50) => SP
//* sprite size (10, 10) => SP
//* sprite size (screen size) => Most of the time SS
//*/
//void RenderTest::Run()
//{
//    const int screenWidth = 800;
//    const int screenHeight = 800;
//    Camera cam = Camera(4000, 4000, Vector3());
//
//    Screen screen = Screen(screenWidth, screenHeight, 10, 3, 6);
//    Screen::SetActiveScreen_A(&screen);
//
//    Sprite* sprite = new Sprite(1, 1);
//
//    std::cout << "TEST 0" << std::endl;
//    RenderTest::TimeSpriteDraw(RenderSystem::DrawSprite_SP, Vector3(), sprite, &cam, 1000, "DrawSprite_SP");
//    RenderTest::TimeSpriteDraw(RenderSystem::DrawSprite_SS, Vector3(), sprite, &cam, 1000, "DrawSprite_SS");
//
//    delete sprite;
//
//    // TEST 1
//    sprite = new Sprite(50, 50);
//
//    std::cout << "TEST 1" << std::endl;
//    RenderTest::TimeSpriteDraw(RenderSystem::DrawSprite_SP, Vector3(), sprite, &cam, 1000, "DrawSprite_SP");
//    RenderTest::TimeSpriteDraw(RenderSystem::DrawSprite_SS, Vector3(), sprite, &cam, 1000, "DrawSprite_SS");
//
//    delete sprite;
//
//    // TEST 2
//    sprite = new Sprite(10, 10);
//
//    std::cout << "TEST 2" << std::endl;
//    RenderTest::TimeSpriteDraw(RenderSystem::DrawSprite_SP, Vector3(), sprite, &cam, 1000, "DrawSprite_SP");
//    RenderTest::TimeSpriteDraw(RenderSystem::DrawSprite_SS, Vector3(), sprite, &cam, 1000, "DrawSprite_SS");
//
//    delete sprite;
//
//    // TEST 3
//    sprite = new Sprite(screenWidth, screenHeight);
//
//    std::cout << "TEST 3" << std::endl;
//    RenderTest::TimeSpriteDraw(RenderSystem::DrawSprite_SP, Vector3(), sprite, &cam, 1000, "DrawSprite_SP");
//    RenderTest::TimeSpriteDraw(RenderSystem::DrawSprite_SS, Vector3(),sprite, &cam, 1000, "DrawSprite_SS");
//
//    delete sprite;
//    
//    std::vector<Entity*> entities;
//    CreateRandomEntitys(&entities, 20, screenWidth, screenHeight);
//    
//    // TEST 4
//    std::cout << "TEST 4" << std::endl;
//    RenderTest::TimeSpritesDraw(RenderSystem::DrawSprites_SP, entities, 500, "DrawSprites_SP");
//    RenderTest::TimeSpritesDraw(RenderSystem::DrawSprites_SS, entities, 500, "DrawSprites_SS");
//
//    entities.clear();
//
//    CreateRandomEntitys(&entities, 100, screenWidth, screenHeight);
//    std::cout << "TEST 5" << std::endl;
//
//    RenderTest::TimeSpritesDraw(RenderSystem::DrawSprites_SP, entities, 500, "DrawSprites_SP");
//
//    RenderTest::TimeSpritesDraw(RenderSystem::DrawSprites_SS, entities, 500, "DrawSprites_SS");
//
//    entities.clear();
//}
//
//int RenderTest::GetRandomNumberInRange(int lower_bound, int upper_bound) {
//    std::random_device rd;  // Obtain a random number from hardware
//    std::mt19937 eng(rd());  // Standard mersenne_twister_engine seeded with rd()
//
//    // Define the distribution for the random number within the range
//    std::uniform_int_distribution<> distribution(lower_bound, upper_bound);
//
//    // Generate a random number within the specified range and return it
//    return distribution(eng);
//}
//
//void RenderTest::CreateRandomEntitys(std::vector<Entity*>* entities, int amount, int width, int height)
//{
//    for (int i = 0; i < amount; i++)
//    {
//        Entity* entity = new Entity();
//
//        int widthR = GetRandomNumberInRange(1, width);
//        int heightR = GetRandomNumberInRange(1, height);
//        int colorR = GetRandomNumberInRange(1, 16);
//
//        entity->AddComponent<Sprite>(widthR, heightR, colorR);
//
//        entities->push_back(entity);
//    }
//}
//
//void RenderTest::TimeSpriteDraw(std::function<void(const Vector3, Sprite*, Camera*)> drawFunction, const Vector3 vec, Sprite* sprite, Camera* cam, int n, const std::string& message)
//{
//    double total_time = 0;
//
//    for (int i = 0; i < n; ++i) {
//        auto start = std::chrono::high_resolution_clock::now();
//
//        drawFunction(vec, sprite, cam);
//
//        auto end = std::chrono::high_resolution_clock::now();
//        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
//
//        total_time += static_cast<double>(duration.count());
//    }
//
//    double avg_time = total_time / n;
//    std::cout << "Average time taken for " << message << " was " << avg_time << " microseconds." << std::endl;
//}
//
//void RenderTest::TimeSpritesDraw(std::function<void(const std::vector<Entity*>&)> drawFunction, const std::vector<Entity*>& entities, int n, const std::string& message)
//{
//    double total_time = 0;
//
//    for (int i = 0; i < n; ++i) {
//        auto start = std::chrono::high_resolution_clock::now();
//
//        drawFunction(entities);
//
//        auto end = std::chrono::high_resolution_clock::now();
//        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
//
//       // std::cout << message << " drew " << n << " sprites in " << static_cast<double>(duration.count()) << " microseconds." << std::endl;
//
//
//        total_time += static_cast<double>(duration.count());
//    }
//
//    double avg_time = total_time / n;
//    std::cout << "Average time taken for " << message << " was " << avg_time << " microseconds." << std::endl;
//}
//
//
//
//
