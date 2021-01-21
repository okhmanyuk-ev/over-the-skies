#pragma once

#include <shared/all.h>
#include "window.h"
#include "helpers.h"

namespace hcg001
{
	class OptionsWindow : public Scene::Actionable<Window>
	{
	public:
		OptionsWindow();

	protected:
		void onCloseBegin() override;

	private:
		std::shared_ptr<Helpers::TextInputField> mNicknameInputField;
	};
}