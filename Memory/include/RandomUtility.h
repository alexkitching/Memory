#pragma once

#include <random>
#include <ctime>


class Random
{
public:

	template<typename T>
	static T IntRange(T a_min, T a_max)
	{
		// Seed
		std::random_device rd; // obtain a random number from hardware
		std::mt19937 eng(rd()); // seed the generator
		std::uniform_int_distribution<T> distr(a_min, a_max); // define the range
		return distr(eng);
	}

	template<typename T>
	static T IntRangeWithSeed(T a_min, T a_max, unsigned a_seed)
	{
		// Seed
		std::mt19937 eng(a_seed); // seed the generator
		std::uniform_int_distribution<T> distr(a_min, a_max); // define the range
		return distr(eng);
	}
	
};