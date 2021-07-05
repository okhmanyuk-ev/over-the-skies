#pragma once

#include <shared/all.h>
#include "profile.h"

#define ACHIEVEMENTS ENGINE->getSystem<hcg001::Achievements>()

namespace hcg001
{
	class Achievements
	{
	public:
		struct Item
		{
			std::string name;
			std::string trigger;
			int required;
		};

	public:
		Achievements();

	public:
		void hit(const std::string& trigger, int count = 1);

	public:
		const auto& getItems() const { return mItems; }
		std::optional<Item> getItemByName(const std::string& name);
		auto& getProgress(const std::string& name) { return PROFILE->getAchievementsProgress()[name]; }
		auto getRequired(const std::string& name) { return getItemByName(name)->required; }

	private:
		std::vector<Item> mItems;
	};
}
