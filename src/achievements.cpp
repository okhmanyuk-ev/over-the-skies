#include "achievements.h"
#include "profile.h"

using namespace hcg001;

Achievements::Achievements()
{
	auto json = JSON("achievements.json");

	for (auto field : json)
	{
		Item item;
		item.name = field["name"];
		item.trigger = field["trigger"];
		item.required = field["required"];
		item.reward = field["reward"];
		mItems.push_back(item);		
	}
}

void Achievements::hit(const std::string& trigger, int count)
{
	for (auto& item : mItems)
	{
		if (item.trigger != trigger)
			continue;

		auto& progress = getProgress(item.name);

		bool earned = progress >= item.required;

		if (earned)
			continue;

		progress += count;

		if (progress < item.required)
			continue;

		progress = item.required;
		EVENT->emit(AchievementEarnedEvent{item});
	}
}

std::optional<Achievements::Item> Achievements::getItemByName(const std::string& name)
{
	for (auto& item : mItems)
	{
		if (item.name != name)
			continue;

		return item;
	}

	return std::nullopt;
}
