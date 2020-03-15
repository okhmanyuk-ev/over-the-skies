#pragma once

#include <map>
#include <renderer/texture.h>

namespace hcg001
{
	enum class Skin
	{
		Ball,
		Snowflake,
		Basketball,
		Football,
		Vynil
	};

	const inline int SkinCount = 5;

	const inline std::map<Skin, std::string> SkinPath = { 
		{ Skin::Ball, "textures/skins/ball.png" },
		{ Skin::Snowflake, "textures/skins/snowflake.png" },
		{ Skin::Basketball, "textures/skins/basketball.png" },
		{ Skin::Football, "textures/skins/football.png" },
		{ Skin::Vynil, "textures/skins/vynil.png" },
	};

	const inline std::map<Skin, int> SkinCost = {
		{ Skin::Ball, 0 },
		{ Skin::Snowflake, 10 },
		{ Skin::Basketball, 25 },
		{ Skin::Football, 50 },
		{ Skin::Vynil, 75 },
		// 100
		// 150
		// 200
	};
}
