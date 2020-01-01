#pragma once

#include <map>
#include <Renderer/texture.h>

namespace hcg001
{
	enum class Skin
	{
		Ball,
		Snowflake,
	//	Third,
	//	Fourth
	};

	const inline int SkinCount = 4;

	const inline std::map<Skin, std::string> SkinPath = { 
		{ Skin::Ball, "textures/skins/ball.png" },
		{ Skin::Snowflake, "textures/skins/snowflake.png" },
	//	{ Skin::Third, "textures/skins/third.png" },
	//	{ Skin::Fourth, "textures/skins/fourth.png" },
	};

	const inline std::map<Skin, int> SkinCost = {
		{ Skin::Ball, 0 },
		{ Skin::Snowflake, 10 },
	//	{ Skin::Third, 25 },
	//	{ Skin::Fourth, 50 }
	};
}
