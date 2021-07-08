#pragma once

#include "window.h"

namespace hcg001
{
	class AchievementsWindow : public Window
	{
	private:
		class Item;

	public:
		AchievementsWindow();
	};

	class AchievementsWindow::Item : public Scene::Rectangle
	{
	public:
		static inline const float VerticalMargin = 8.0f;

	public:
		Item(int num, const std::string& name);
	};
}