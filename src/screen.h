#pragma once

#include <shared/all.h>

namespace hcg001
{
	class Screen : public Scene::RenderLayer<Shared::SceneManager::Screen>, public std::enable_shared_from_this<Screen>
	{
	public:
		Screen();

	protected:
		void onEnterBegin() override;
		void onEnterEnd() override;
		void onLeaveBegin() override;
		void onLeaveEnd() override;
		void onWindowAppearing() override;
		void onWindowDisappearing() override;

	protected:
		std::unique_ptr<Actions::Action> createEnterAction() override;
		std::unique_ptr<Actions::Action> createLeaveAction() override;

	public:
		auto getContent() { return mContent; }
		auto getGui() { return mGui; }

	private:
		std::shared_ptr<Scene::Node> mContent;
		std::shared_ptr<Shared::SceneHelpers::SafeArea> mGui;
	};
}