//
//  Random.cxx
//  RemoteInput
//
//  Created by Brandon on 2019-12-29.
//  Copyright © 2019 XIO. All rights reserved.
//

#include "Random.hxx"

std::shared_ptr<Random> Random::instance()
{
	static std::shared_ptr<Random> instance{new Random()};
	return instance;
}

std::size_t Random::generate_random_int(std::size_t min, std::size_t max)
{
	static std::uniform_int_distribution<decltype(rng)::result_type> dist{min, max};
	return dist(rng, decltype(dist)::param_type(min, max));
}

float Random::generate_random_float(float min, float max)
{
	static std::uniform_real_distribution<> dist{min, std::nextafter(max, std::numeric_limits<float>::max())};
	return dist(rng, decltype(dist)::param_type(min, std::nextafter(max, std::numeric_limits<float>::max())));
}

double Random::generate_random_double(double min, double max)
{
	static std::uniform_real_distribution<> dist{min, std::nextafter(max, std::numeric_limits<double>::max())};
	return dist(rng, decltype(dist)::param_type(min, std::nextafter(max, std::numeric_limits<double>::max())));
}
