#pragma once

#include <random>

//------------
// Description
//--------------
// Random Utility Class used for Generating Random Integers/Bools/Floats with/without a seed
// Used Seed for Dev purposes, memory allocation patterns in scenarios would be the same each run.
//------------

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

	template<typename T>
	static T FloatRange(T a_min, T a_max)
	{
		// Seed
		std::random_device rd; // obtain a random number from hardware
		std::mt19937 eng(rd()); // seed the generator
		std::uniform_real_distribution<T> distr(a_min, a_max);
		return distr(eng);
	}

	template<typename T>
	static T FloatRangeWithSeed(T a_min, T a_max, unsigned int a_seed)
	{
		// Seed
		std::mt19937 eng(a_seed); // seed the generator
		std::uniform_real_distribution<T> distr(a_min, a_max);
		return distr(eng);
	}

	static bool Bool()
	{
		return IntRange(0, 1);
	}
	
	static bool BoolWithSeed(unsigned a_seed)
	{
		return IntRangeWithSeed(0, 1, a_seed);
	}
	
};