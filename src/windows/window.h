#pragma once

#include <shared/all.h>
#include "helpers.h"

namespace hcg001
{
	class Window : public Scene::Clickable<Shared::SceneHelpers::Backshaded<Shared::SceneManager::Window>>
	{
	public:
		Window();

	public:
		void onOpenEnd() override;
		void onCloseBegin() override;

	public:
		std::unique_ptr<Actions::Action> createOpenAction() override;
		std::unique_ptr<Actions::Action> createCloseAction() override;

	public:
		auto getContent() { return mContent; }

		auto getCloseOnMissclick() const { return mCloseOnMissclick; }
		void setCloseOnMissclick(bool value) { mCloseOnMissclick = value; }

	public:
		auto getFadeAlpha() const { return mFadeAlpha; }
		void setFadeAlpha(float value) { mFadeAlpha = value; }

	private:
		std::shared_ptr<Scene::Node> mContent;
		bool mCloseOnMissclick = true;
		float mFadeAlpha = 0.5f;
	};

	class StandardWindow : public Window
	{
	public:
		StandardWindow(bool has_close_button = true);

	public:
		auto getBackground() const { return mBackground; }
		auto getBody() const { return mBody; }
		auto getTitle() const { return mTitle; }

	private:
		std::shared_ptr<Scene::ClippableStencil<Scene::Rectangle>> mBackground;
		std::shared_ptr<Scene::Node> mBody;
		std::shared_ptr<Helpers::Label> mTitle;
	};
}
