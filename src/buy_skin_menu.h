#pragma once

#include <shared/all.h>
#include "screen.h"
#include "skin.h"
#include "helpers.h"

namespace hcg001
{
	class BuySkinMenu : public Screen
	{
	public:
		BuySkinMenu(Skin skin);

	private:
		std::shared_ptr<Scene::Label> mTitle = nullptr;
		std::shared_ptr<Helpers::Button> mBuyButton = nullptr;
		std::shared_ptr<Helpers::Button> mCancelButton = nullptr;
	};
}
