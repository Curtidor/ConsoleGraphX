#include "random_numbers.h"
#include <random>

int RandomNumberGenerator::GenerateRandomIntInRange(int min, int max) {
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<> dis(min, max);

	return dis(gen);
}



float RandomNumberGenerator::GenerateRandomFloatInRange(float min, float max) {
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_real_distribution<float> dis(min, max);

	return dis(gen);
}
