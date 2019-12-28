#pragma once

#include "screen.h"

namespace hcg001
{
	class GameplayScreen : public Scene::Actionable<Screen>
	{
	public:
		GameplayScreen();

	protected:
		void touch(Touch type, const glm::vec2& pos) override;

	private:
		std::shared_ptr<Scene::Label> mReadyLabel;

	public:
		void setReadyCallback(std::function<void()> value) { mReadyCallback = value; }

	private:
		std::function<void()> mReadyCallback = nullptr;
		bool mReady = false;
	};
}