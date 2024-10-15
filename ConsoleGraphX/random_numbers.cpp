#include "CGXPCH.h"
#include <random>
#include "random_numbers.h"

int RandomNumberGenerator::GenerateRandomIntInRange(int min, int max) 
{
    std::uniform_int_distribution<> dis(min, max);
    return dis(gen);
}

float RandomNumberGenerator::GenerateRandomFloatInRange(float min, float max) 
{
    std::uniform_real_distribution<float> dis(min, max);
    return dis(gen);
}
