#pragma once

#include <Shared/all.h>

namespace hcg001::Helpers
{
	const float ButtonRounding = 0.33f;

	inline bool Chance(float normalized_percent)
	{
		return glm::linearRand(0.0f, 1.0f) <= normalized_percent;
	}
}