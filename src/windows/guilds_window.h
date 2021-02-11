#pragma once

#include "window.h"

namespace hcg001
{
	class GuildsWindow : public StandardWindow
	{
	public:
		class Item;

	public:
		GuildsWindow();
	};

	class GuildsWindow::Item : public Scene::Rectangle
	{
	public:
		Item();
	};
}