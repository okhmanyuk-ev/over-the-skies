#pragma once

#include <shared/all.h>
#include "profile.h"
#include "windows/daily_reward_window.h"

namespace hcg001
{
	inline void ShowCheatsMenu()
	{
#if !defined(BUILD_DEVELOPER)
		return;
#endif	

		ImGui::Begin("dev", nullptr, ImGui::User::ImGuiWindowFlags_ControlPanel);
		ImGui::SetWindowPos(ImGui::User::BottomLeftCorner());

		static bool Enabled = false;

		if (Enabled)
		{
			if (ImGui::Button("CLEAR PROFILE"))
			{
				PROFILE->clear();
			}

			if (ImGui::Button("RUBIES +10"))
			{
				PROFILE->setRubies(PROFILE->getRubies() + 10);
			}

			if (ImGui::Button("DAILY REWARD WINDOW"))
			{
				auto window = std::make_shared<DailyRewardWindow>(2);
				SCENE_MANAGER->pushWindow(window);
			}
		}

		ImGui::Checkbox("DEV", &Enabled);
		ImGui::End();
	}
}