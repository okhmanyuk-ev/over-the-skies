#pragma once

#include "window.h"

namespace hcg001
{
	class AchievementsWindow : public Window
	{
	public:
		static inline std::shared_ptr<Helpers::RubiesIndicator> RubiesIndicator = nullptr;

	private:
		class Item;

	public:
		AchievementsWindow();
		~AchievementsWindow();

	protected:	
		void onOpenEnd() override;
		void onCloseBegin() override;
	};

	class AchievementsWindow::Item : public Scene::Cullable<Scene::Rectangle>
	{
	public:
		static inline const float VerticalMargin = 8.0f;

	public:
		Item(int num, const std::string& name);

	public:
		void refresh(bool anim = false);

	private:
		std::shared_ptr<Scene::Adaptive<Scene::Sprite>> mCheck;
		std::shared_ptr<Helpers::Button> mButton;
		std::shared_ptr<Scene::Node> mButtonHolder;
		std::string mName;
	};
}