#pragma once

#include <map>
#include <renderer/texture.h>

namespace hcg001
{
	enum class Skin
	{
		Ball,
		Snowflake
	};

	const inline int SkinCount = 4;

	const inline std::map<Skin, std::string> SkinPath = { 
		{ Skin::Ball, "textures/skins/ball.png" },
		{ Skin::Snowflake, "textures/skins/snowflake.png" },
	};

	const inline std::map<Skin, int> SkinCost = {
		{ Skin::Ball, 0 },
		{ Skin::Snowflake, 50 },
	};
}
