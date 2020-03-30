#pragma once

#include <shared/all.h>

#include "skin.h"

#define PROFILE ENGINE->getSystem<hcg001::Profile>()

namespace hcg001
{
	class Profile
	{
	public:
		struct RubiesChangedEvent { };

	public:
		void load();
		void save();
		void saveAsync();
		void clear();

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

		auto getSkins() const { return mSkins; }
		void setSkins(std::set<int> value) { mSkins = value; }

		auto getDailyRewardDay() const { return mDailyRewardDay; }
		void setDailyRewardDay(int value) { mDailyRewardDay = value; }

		auto getDailyRewardTime() const { return mDailyRewardTime; }
		void setDailyRewardTime(long long value) { mDailyRewardTime = value; }

	private:
		int mHighScore = 0;
		int mRubies = 0;
		std::set<int> mSkins = { 0 };
		int mDailyRewardDay = 0;
		long long mDailyRewardTime = 0;
	};
}
