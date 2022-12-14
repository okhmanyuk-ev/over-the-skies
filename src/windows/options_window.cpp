#include "options_window.h"

using namespace hcg001;

OptionsWindow::OptionsWindow()
{
	getBackground()->setSize({ 314.0f, 128.0f });
	getTitle()->setText(LOCALIZE("OPTIONS_WINDOW_TITLE"));

	auto nickname_label = std::make_shared<Helpers::Label>();
	nickname_label->setFontSize(16.0f);
	nickname_label->setPosition({ 16.0f, 24.0f });
	nickname_label->setPivot({ 0.0f, 0.5f });
	nickname_label->setText(LOCALIZE("OPTIONS_WINDOW_NICKNAME"));
	getBody()->attach(nickname_label);

	mNicknameInputField = std::make_shared<Helpers::TextInputField>(LOCALIZE("INPUT_NICK_NAME"));
	mNicknameInputField->setPosition({ 128.0f, 24.0f });
	mNicknameInputField->setSize({ 174.0f, 24.0f });
	mNicknameInputField->setPivot({ 0.0f, 0.5f });
	mNicknameInputField->getLabel()->setText(PROFILE->getNickName());
	getBody()->attach(mNicknameInputField);

	PROFILE->setNicknameChanged(true); // that means that user have been seen his nickname, and no need to show him a nickname change window

	auto ok_button = std::make_shared<Helpers::Button>();
	ok_button->getLabel()->setText(LOCALIZE("WINDOW_OK"));
	ok_button->setClickCallback([] {
		SCENE_MANAGER->popWindow();
	});
	ok_button->setAnchor({ 0.5f, 1.0f });
	ok_button->setPivot({ 0.5f, 0.5f });
	ok_button->setY(-24.0f);
	ok_button->setSize({ 128.0f, 28.0f });
	getBody()->attach(ok_button);
}

void OptionsWindow::onCloseBegin()
{
	Window::onCloseBegin();

	PROFILE->setNickName(mNicknameInputField->getLabel()->getText());
	PROFILE->save();
}
