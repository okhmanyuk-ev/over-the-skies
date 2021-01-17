#pragma once

#include <shared/all.h>
#include "helpers.h"

namespace hcg001
{
	class Application : public Shared::Application,
		public Common::FrameSystem::Frameable,
		public Common::Event::Listenable<Helpers::PrintEvent>,
		public Common::Event::Listenable<Shared::Profile::ProfileSavedEvent>
	{
	public:
		Application();
		~Application();

	private:
		void initialize();
		void frame() override;
		void addRubies(int count);
		void tryShowDailyReward();
		void adaptToScreen(std::shared_ptr<Scene::Node> node);

	private:
		void onEvent(const Helpers::PrintEvent& e) override;
		void onEvent(const Shared::Profile::ProfileSavedEvent& e) override;
	};
}
