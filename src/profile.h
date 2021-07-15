#pragma once

#include <shared/all.h>
#include "skin.h"

#define PROFILE ENGINE->getSystem<hcg001::Profile>()

namespace hcg001
{
	class Profile : public Shared::Profile
	{
	public:
		static const int inline NoneGuild = -1;

	public:
		struct RubiesChangedEvent { };

	public:
		void read(const nlohmann::json& json) override;
		void write(nlohmann::json& json) override;

	protected:
		void makeDefault() override;

	public:
		bool isSkinLocked(int value) const { return mSkins.count(value) == 0; }
		bool isSkinLocked(Skin value) const { return isSkinLocked((int)value); }

		void increaseRubies(int count) { setRubies(getRubies() + count); }
		void decreaseRubies(int count) { setRubies(getRubies() - count); }

	public:
		int getHighScore() const { return mHighScore; }
		void setHighScore(int value) { mHighScore = value; }

		int getRubies() const { return mRubies; }
		void setRubies(int value);

		auto getCurrentSkin() const { return mCurrentSkin; }
		void setCurrentSkin(Skin value) { mCurrentSkin = value; }

		auto getSkins() const { return mSkins; }
		void setSkins(std::set<int> value) { mSkins = value; }

		auto getDailyRewardDay() const { return mDailyRewardDay; }
		void setDailyRewardDay(int value) { mDailyRewardDay = value; }

		auto getDailyRewardTime() const { return mDailyRewardTime; }
		void setDailyRewardTime(long long value) { mDailyRewardTime = value; }

		auto getNickName() const { return mNickName; }
		void setNickName(const utf8_string& value) { mNickName = value; }

		// guild_id data not stored in local profile,
		// it will receive from server every game start
		int getGuildId() const { return mGuildId; }
		void setGuildId(int value) { mGuildId = value; }

		bool isInGuild() const { return getGuildId() != NoneGuild; }

		auto& getAchievementsProgress() { return mAchievementsProgress; }

		bool isNicknameChanged() const { return mNicknameChanged; }
		void setNicknameChanged(bool value) { mNicknameChanged = value; }

		bool isAchievementRewardTaken(const std::string& achievement_id) const;
		void achievementRewardTake(const std::string& achievement_id);

	private:
		int mHighScore = 0;
		int mRubies = 0;
		Skin mCurrentSkin = Skin::Ball;
		std::set<int> mSkins = { 0 };
		int mDailyRewardDay = 0;
		long long mDailyRewardTime = 0;
		utf8_string mNickName = "Guest";
		int mGuildId = NoneGuild;
		std::map<std::string, int> mAchievementsProgress;
		bool mNicknameChanged = false;
		std::set<std::string> mAchievementRewardTaken;
	};
}
