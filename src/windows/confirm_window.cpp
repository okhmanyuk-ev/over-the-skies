#include "confirm_window.h"

using namespace hcg001;

ConfirmWindow::ConfirmWindow(const std::wstring& title, const std::wstring& yes_text,
	const std::wstring& no_text) : Window()
{
	getBackground()->setSize({ 314.0f, 96.0f });
	getTitle()->setText(title);

	glm::vec2 ButtonSize = { 96.0f + 16.0f, 28.0f };

	auto yes_button = std::make_shared<Helpers::Button>();
	yes_button->getLabel()->setText(yes_text);
	yes_button->setAnchor(0.5f);
	yes_button->setPivot({ 1.125f, 0.5f });
	yes_button->setSize(ButtonSize);
	yes_button->setClickCallback([this] {
		if (mYesCallback)
			mYesCallback();
	});
	getBody()->attach(yes_button);

	auto no_button = std::make_shared<Helpers::Button>();
	no_button->getLabel()->setText(no_text);
	no_button->setAnchor(0.5f);
	no_button->setPivot({ -0.125f, 0.5f });
	no_button->setSize(ButtonSize);
	no_button->setClickCallback([this] {
		if (mNoCallback)
			mNoCallback();
	});
	getBody()->attach(no_button);
}
