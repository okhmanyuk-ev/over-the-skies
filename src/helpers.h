#pragma once

#include <Shared/all.h>

namespace hcg001::Helpers
{
	inline bool Chance(float normalized_percent)
	{
		return glm::linearRand(0.0f, 1.0f) <= normalized_percent;
	}
}