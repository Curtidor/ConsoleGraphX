#pragma once
#include <random>

class RandomNumberGenerator {
public:
    // Generates a random integer in the range [min, max]
    static int GenerateRandomIntInRange(int min, int max);

    // Generates a random float in the range [min, max]
    static float GenerateRandomFloatInRange(float min, float max);

private:
    static inline std::minstd_rand gen = std::minstd_rand(std::random_device{}());
};
