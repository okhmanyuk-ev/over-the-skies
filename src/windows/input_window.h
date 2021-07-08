#pragma once

#include <shared/all.h>
#include "window.h"
#include "helpers.h"

namespace hcg001
{
	class InputWindow : public Window,
		public Common::Event::Listenable<Platform::System::VirtualKeyboardTextChanged>,
		public Common::Event::Listenable<Platform::System::VirtualKeyboardEnterPressed>,
		public Common::Event::Listenable<Platform::Input::Keyboard::Event>
	{
	public:
		using ChangeTextCallback = std::function<void(const utf8_string&)>;

	public:
		InputWindow(const utf8_string& text, ChangeTextCallback changeTextCallback);

	private:
		void onEvent(const Platform::System::VirtualKeyboardTextChanged& e) override;
		void onEvent(const Platform::System::VirtualKeyboardEnterPressed& e) override;
		void onEvent(const Platform::Input::Keyboard::Event& e) override;

	protected:
		void onOpenBegin() override;
		void onCloseBegin() override;

	private:
		static inline int StackCount = 0; // should be maximum 1

	private:
		std::shared_ptr<Helpers::Label> mLabel;
		ChangeTextCallback mChangeTextCallback;
		std::shared_ptr<Helpers::Button> mApplyButton;
		std::shared_ptr<Helpers::Button> mCancelButton;
	};
}