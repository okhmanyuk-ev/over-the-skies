#pragma once

#include "screen.h"

namespace hcg001
{
	class ReadyMenu : public Scene::Clickable<Screen>
	{
	public:
		ReadyMenu();

	public:
		void setReadyCallback(std::function<void()> value) { mReadyCallback = value; }

	private:
		std::function<void()> mReadyCallback = nullptr;
	};
}