#pragma once

#include "screen.h"

namespace hcg001
{
	class GameoverMenu : public Scene::Tappable<Screen>
	{
	public:
		GameoverMenu(int score);
	};
}
