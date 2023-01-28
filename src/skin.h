#pragma once

#include <map>
#include <string>

namespace hcg001
{
	enum class Skin
	{
		Ball,
		Snowflake,
		Basketball,
		Football,
		Vynil,
	//	Payable // TODO: rename
	};

	const inline int SkinCount = 6;

	const inline std::map<Skin, std::string> SkinPath = { 
		{ Skin::Ball, "textures/skins/ball.png" },
		{ Skin::Snowflake, "textures/skins/snowflake.png" },
		{ Skin::Basketball, "textures/skins/basketball.png" },
		{ Skin::Football, "textures/skins/football.png" },
		{ Skin::Vynil, "textures/skins/vynil.png" },
	//	{ Skin::Payable, "textures/skins/ball.png" }
	};

	const inline std::map<Skin, int> SkinCost = {
		{ Skin::Ball, 0 },
		{ Skin::Snowflake, 10 },
		{ Skin::Basketball, 25 },
		{ Skin::Football, 50 },
		{ Skin::Vynil, 75 },
	//	{ Skin::Payable, 0 }
		// 100
		// 150
		// 200
	};
}
