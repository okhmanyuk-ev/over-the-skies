#pragma once

#include <shared/all.h>

namespace hcg001
{
	class Player : public Scene::Actionable<Scene::Node>
	{
	public:
		Player()
		{
			setSize(18.0f);
			setPivot({ 0.5f, 0.5f });

			mSprite = std::make_shared<Scene::Sprite>(); // we should use sprite as child and rotate only this child
			mSprite->setStretch(1.0f);
			mSprite->setAnchor({ 0.5f, 0.5f });
			mSprite->setPivot({ 0.5f, 0.5f });
			attach(mSprite);
		}

		auto getSprite() { return mSprite; }

	private:
		std::shared_ptr<Scene::Sprite> mSprite;
	};
}