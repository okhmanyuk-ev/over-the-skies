#pragma once

#include <shared/all.h>
#include "hud.h"

namespace hcg001::Helpers
{
	const float ButtonRounding = 0.33f;

	inline std::shared_ptr<Hud> gHud = nullptr;

	inline bool Chance(float normalized_percent)
	{
		return glm::linearRand(0.0f, 1.0f) <= normalized_percent;
	}
}