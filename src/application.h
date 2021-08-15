#pragma once

#include <shared/all.h>
#include "helpers.h"
#include "client.h"
#include "achievements.h"

namespace hcg001
{
	class Application : public Shared::Application,
		public Common::FrameSystem::Frameable,
		public Common::Event::Listenable<Shared::Profile::ProfileSavedEvent>,
		public Common::Event::Listenable<Achievements::AchievementEarnedEvent>
	{
	public:
		Application();
		~Application();

	private:
		void initialize();
		void onFrame() override;
		void addRubies(int count);
		void tryShowDailyReward();
		void adaptToScreen(std::shared_ptr<Scene::Node> node);
		void showCheats();

	private:
		void onEvent(const Shared::Profile::ProfileSavedEvent& e) override;
		void onEvent(const Achievements::AchievementEarnedEvent& e) override;		
	};
}
