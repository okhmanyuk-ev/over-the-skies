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

	if (json.contains("current_skin"))
	{
		mCurrentSkin = (Skin)json["current_skin"].get<int>();
	}

	if (json.contains("achievements_progress"))
	{
		for (auto& field : json["achievements_progress"])
		{
			std::string name = field["name"];
			int progress = field["progress"];
			mAchievementsProgress[name] = progress;
		}
	}

	mNicknameChanged = json.value("nickname_changed", false);

	if (json.contains("achievement_reward_taken"))
		mAchievementRewardTaken = json["achievement_reward_taken"].get<std::set<std::string>>();

}

void Profile::write(nlohmann::json& json)
{
	json["highscore"] = mHighScore;
	json["rubies"] = mRubies;
	json["skins"] = mSkins;
	json["dailyreward_day"] = mDailyRewardDay;
	json["dailyreward_time"] = mDailyRewardTime;
	json["nickname"] = std::vector<utf8_string::value_type>(mNickName.begin(), mNickName.end());
	json["current_skin"] = (int)mCurrentSkin;
	json["nickname_changed"] = mNicknameChanged;

	for (const auto& [name, progress] : mAchievementsProgress)
	{
		json["achievements_progress"].push_back({
			{ "name", name },
			{ "progress", progress }
		});
	}

	json["achievement_reward_taken"] = mAchievementRewardTaken;
}

void Profile::makeDefault()
{
	mHighScore = 0;
	setRubies(0);
	mSkins = { 0 };
	mNickName = "Guest";
	mAchievementsProgress.clear();
	mNicknameChanged = false;
	mAchievementRewardTaken.clear();
}

void Profile::setRubies(int value)
{
	if (mRubies == value)
		return;

	mRubies = value;
	EVENT->emit(RubiesChangedEvent());
}

bool Profile::isAchievementRewardTaken(const std::string& achievement_id) const
{
	return mAchievementRewardTaken.count(achievement_id);
}

void Profile::achievementRewardTake(const std::string& achievement_id)
{
	mAchievementRewardTaken.insert(achievement_id);
}