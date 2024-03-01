#pragma once
#include <iostream>
#include <random>

class RandomNumberGenerator 
{
public:
	static int GenerateRandomIntInRange(int min, int max);
	static float GenerateRandomFloatInRange(float min, float max);
		
};