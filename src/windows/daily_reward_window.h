#pragma once

#include <shared/all.h>
#include "window.h"

namespace hcg001
{
	class DailyRewardWindow : public Window
	{
	public:
		const static inline std::map<int, int> DailyRewardMap = {
			{ 1, 1 },
			{ 2, 3 },
			{ 3, 6 },
			{ 4, 9 },
			{ 5, 12 },
			{ 6, 15 },
			{ 7, 20 }
		};

	public:
		DailyRewardWindow(int current_day);

	public:
		void setClaimCallback(std::function<void()> value) { mClaimCallback = value; }

	private:
		std::function<void()> mClaimCallback;
	};
}