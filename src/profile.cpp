#include "profile.h"

using namespace hcg001;

void Profile::load()
{
	auto path = PLATFORM->getAppFolder() + "save.bson";

	if (!Platform::Asset::Exists(path, Platform::Asset::Path::Absolute))
		return;

	auto json_file = Platform::Asset(path, Platform::Asset::Path::Absolute);
	auto json = nlohmann::json::from_bson(std::string((char*)json_file.getMemory(), json_file.getSize()));

	if (json.contains("highscore"))
		mHighScore = json["highscore"];

	if (json.contains("rubies"))
		mRubies = json["rubies"];

	if (json.contains("skins"))
		mSkins = json["skins"].get<std::set<int>>();

	if (json.contains("dailyreward_day"))
		mDailyRewardDay = json["dailyreward_day"];

	if (json.contains("dailyreward_time"))
		mDailyRewardTime = json["dailyreward_time"];
}

void Profile::save()
{
	auto json = nlohmann::json();
	json["highscore"] = mHighScore;
	json["rubies"] = mRubies;
	json["skins"] = mSkins;
	json["dailyreward_day"] = mDailyRewardDay;
	json["dailyreward_time"] = mDailyRewardTime;
	auto bson = nlohmann::json::to_bson(json);
	Platform::Asset::Write(PLATFORM->getAppFolder() + "save.bson", bson.data(), bson.size(), Platform::Asset::Path::Absolute);
}

void Profile::clear()
{
	mHighScore = 0;
	setRubies(0);
	mSkins = { 0 };
}

void Profile::saveAsync()
{
	TASK->addTask([this] {
		save();
	});
}

void Profile::setRubies(int value)
{
	if (mRubies == value)
		return;

	mRubies = value;
	EVENT->emit(RubiesChangedEvent());
}
