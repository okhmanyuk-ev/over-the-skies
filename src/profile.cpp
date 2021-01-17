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

	if (json.contains("nickname"))
	{
		auto nickname = json["nickname"].get<std::vector<utf8_string::value_type>>();
		mNickName.assign(nickname.begin(), nickname.end());
	}
}

void Profile::write(nlohmann::json& json)
{
	json["highscore"] = mHighScore;
	json["rubies"] = mRubies;
	json["skins"] = mSkins;
	json["dailyreward_day"] = mDailyRewardDay;
	json["dailyreward_time"] = mDailyRewardTime;
	json["nickname"] = std::vector<utf8_string::value_type>(mNickName.begin(), mNickName.end());
}

void Profile::makeDefault()
{
	mHighScore = 0;
	setRubies(0);
	mSkins = { 0 };
	mNickName = "Guest";
}

void Profile::setRubies(int value)
{
	if (mRubies == value)
		return;

	mRubies = value;
	EVENT->emit(RubiesChangedEvent());
}
