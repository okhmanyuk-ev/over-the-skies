#pragma once

#include <shared/all.h>

namespace hcg001
{
	class Screen : public Shared::SceneHelpers::StandardScreen
	{
	public:
		Screen();

	public:
		auto getGui() { return mGui; }

	private:
		std::shared_ptr<Shared::SceneHelpers::SafeArea> mGui;
	};
}