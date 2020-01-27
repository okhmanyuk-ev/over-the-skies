#pragma once

#include <Shared/all.h>
#include "profile.h"

#define GAME_SYSTEMS static_cast<GameSystems*>(ENGINE->getCustomSystem())

#define PROFILE GAME_SYSTEMS->getProfile()

namespace hcg001
{
	class GameSystems
	{
	public:
		auto getProfile() const { return mProfile; }
		void setProfile(Profile* value) { mProfile = value; }

	private:
		Profile* mProfile = nullptr;
	};
}
