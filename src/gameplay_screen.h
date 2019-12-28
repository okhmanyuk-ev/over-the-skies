#pragma once

#include "screen.h"

namespace hcg001
{
	class GameplayScreen : public Scene::Actionable<Scene::Clickable<Screen>>
	{
	public:
		GameplayScreen();

	public:
		void setReadyCallback(std::function<void()> value) { mReadyCallback = value; }

	private:
		std::function<void()> mReadyCallback = nullptr;
		bool mReady = false;
	};
}