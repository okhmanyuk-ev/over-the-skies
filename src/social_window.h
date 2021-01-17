#pragma once

#include <shared/all.h>
#include "window.h"
#include "helpers.h"

namespace hcg001
{
	class SocialWindow : public Scene::Actionable<Window>
	{
	public:
		SocialWindow();

	protected:
		void onCloseBegin() override;

	private:
		std::shared_ptr<Helpers::TextInputField> mNicknameInputField;
	};
}