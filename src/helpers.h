#pragma once

#include <shared/all.h>
#include "hud.h"

namespace hcg001::Helpers
{
	const float ButtonRounding = 0.33f;

	const auto BaseWindowColor = glm::rgbColor(glm::vec3(210.0f, 0.5f, 1.0f));

	inline std::shared_ptr<Hud> gHud = nullptr;

	struct PrintEvent
	{
		std::string text;
	};
}