#pragma once

#include <shared/all.h>
#include "helpers.h"
#include "client.h"

namespace hcg001
{
	class Application : public Shared::Application,
		public Common::FrameSystem::Frameable,
		public Common::Event::Listenable<NetEvents::PrintEvent>,
		public Common::Event::Listenable<Shared::Profile::ProfileSavedEvent>
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
		void inputNickname();

	private:
		void onEvent(const NetEvents::PrintEvent& e) override;
		void onEvent(const Shared::Profile::ProfileSavedEvent& e) override;
	};
}
