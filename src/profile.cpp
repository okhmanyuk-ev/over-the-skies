#include "profile.h"

using namespace hcg001;

void Profile::read(const nlohmann::json& json)
{
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

void Profile::write(nlohmann::json& json)
{
	json["highscore"] = mHighScore;
	json["rubies"] = mRubies;
	json["skins"] = mSkins;
	json["dailyreward_day"] = mDailyRewardDay;
	json["dailyreward_time"] = mDailyRewardTime;
}

void Profile::makeDefault()
{
	mHighScore = 0;
	setRubies(0);
	mSkins = { 0 };
}

void Profile::setRubies(int value)
{
	if (mRubies == value)
		return;

	mRubies = value;
	EVENT->emit(RubiesChangedEvent());
}
