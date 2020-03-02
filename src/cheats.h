#pragma once

#include <Shared/all.h>
#include "defines.h"
#include "profile.h"

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
		}

		ImGui::Checkbox("DEV", &Enabled);
		ImGui::End();
	}
}