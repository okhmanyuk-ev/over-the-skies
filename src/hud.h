#pragma once

#include <shared/all.h>

namespace hcg001
{
	class Hud : public Shared::SceneHelpers::SafeArea
	{
	public:
		Hud();

	public:
		void collectRubyAnim(std::shared_ptr<Scene::Node> ruby);

	private:
		struct
		{
			std::shared_ptr<Scene::Sprite> sprite;
			std::shared_ptr<Scene::Label> label;
		} mRubyScore;
	};
}
