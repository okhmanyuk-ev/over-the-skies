#pragma once

#include "window.h"

namespace hcg001
{
	class ConfirmWindow : public Window
	{
	public:
		ConfirmWindow(const std::wstring& title, const std::wstring& yes_text,
			const std::wstring& no_text);

	public:
		void setYesCallback(std::function<void()> value) { mYesCallback = value; }
		void setNoCallback(std::function<void()> value) { mNoCallback = value; }

	private:
		std::function<void()> mYesCallback = nullptr;
		std::function<void()> mNoCallback = nullptr;
	};
}