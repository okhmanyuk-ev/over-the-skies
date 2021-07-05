#include "achievements.h"
#include "profile.h"

using namespace hcg001;

Achievements::Achievements()
{
	auto json = Common::Helpers::LoadJsonFromAsset({ "achievements.json" });

	for (auto field : json)
	{
		Item item;
		item.name = field["name"];
		item.trigger = field["trigger"];
		item.required = field["required"];
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

		progress += count;

		if (progress > item.required)
			progress = item.required;
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
