#pragma once

#include "window.h"

namespace hcg001
{
	class OptionsWindow : public Window
	{
	public:
		OptionsWindow();

	protected:
		void onCloseBegin() override;

	private:
		std::shared_ptr<Helpers::TextInputField> mNicknameInputField;
	};
}