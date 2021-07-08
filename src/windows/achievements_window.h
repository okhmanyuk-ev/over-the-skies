#pragma once

#include "window.h"

namespace hcg001
{
	class AchievementsWindow : public StandardWindow
	{
	private:
		class Item;

	public:
		AchievementsWindow();
	};

	class AchievementsWindow::Item : public Scene::Rectangle
	{
	public:
		Item(int num, const std::string& name);
	};
}