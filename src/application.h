#pragma once

#include <sky/sky.h>
#include "helpers.h"
#include "achievements.h"

namespace hcg001
{
	class Application :
		public sky::Scheduler::Frameable,
		public sky::Listenable<Achievements::AchievementEarnedEvent>
	{
	public:
		Application();
		~Application();

	private:
		void initialize();
		void onFrame() override;
		void addRubies(int count);
		void tryShowDailyReward();
		void showCheats();

	private:
		void onEvent(const Achievements::AchievementEarnedEvent& e) override;
	};
}
