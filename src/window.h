#pragma once

#include <shared/all.h>

namespace hcg001
{
	class Window : public Scene::Clickable<Shared::SceneHelpers::Backshaded<Shared::SceneManager::Window>>
	{
	public:
		Window()
		{
			setStretch(1.0f);
			setClickCallback([this] {
				if (!mCloseOnMissclick)
					return;

				if (getState() != State::Opened)
					return;

				SCENE_MANAGER->popWindow();
			});

			getBackshadeColor()->setColor({ Graphics::Color::Black, 0.0f });
	
			mBlur = std::make_shared<Scene::Blur>();
			mBlur->setStretch(1.0f);
			mBlur->setRadius(32);
			attach(mBlur);

			mContent = std::make_shared<Scene::Node>();
			mContent->setStretch(1.0f);
			mContent->setAnchor({ 0.5f, -0.5f });
			mContent->setPivot(0.5f);
			mContent->setInteractions(false);
			attach(mContent);
		}

		void onOpenEnd() override
		{
			mContent->setInteractions(true);
		}

		void onCloseBegin() override
		{
			mContent->setInteractions(false);
		}

		std::unique_ptr<Common::Actions::Action> createOpenAction() override
		{
			return Shared::ActionHelpers::MakeParallel(
		//		Shared::ActionHelpers::ChangeAlpha(getBackshadeColor(), 0.5f, 0.5f, Common::Easing::CubicOut),
				Shared::ActionHelpers::ChangeVerticalAnchor(mContent, 0.5f, 0.5f, Common::Easing::CubicOut)
			);
		};

		std::unique_ptr<Common::Actions::Action> createCloseAction() override
		{
			return Shared::ActionHelpers::MakeParallel(
		//		Shared::ActionHelpers::ChangeAlpha(getBackshadeColor(), 0.0f, 0.5f, Common::Easing::CubicIn),
				Shared::ActionHelpers::ChangeVerticalAnchor(mContent, -0.5f, 0.5f, Common::Easing::CubicIn)
			);
		};

	public:
		auto getContent() { return mContent; }

		auto getCloseOnMissclick() const { return mCloseOnMissclick; }
		void setCloseOnMissclick(bool value) { mCloseOnMissclick = value; }

	private:
		std::shared_ptr<Scene::Node> mContent;
		std::shared_ptr<Scene::Blur> mBlur;
		bool mCloseOnMissclick = true;
	};
}
