#include "social_window.h"
#include "helpers.h"

using namespace hcg001;

SocialWindow::SocialWindow()
{
	auto rect = std::make_shared<Scene::ClippableStencil<Scene::Rectangle>>();
	rect->setRounding(12.0f);
	rect->setAbsoluteRounding(true);
	rect->setSize({ 314.0f, 286.0f });
	rect->setAnchor(0.5f);
	rect->setPivot(0.5f);
	rect->setTouchable(true);
	rect->setColor(Helpers::BaseWindowColor / 12.0f);
	getContent()->attach(rect);

	auto header = std::make_shared<Scene::Node>();
	header->setHorizontalStretch(1.0f);
	header->setHeight(36.0f);
	rect->attach(header);

	auto footer = std::make_shared<Scene::Node>();
	footer->setHeight(48.0f);
	footer->setHorizontalStretch(1.0f);
	footer->setAnchor({ 0.5f, 1.0f });
	footer->setPivot({ 0.5f, 1.0f });
	rect->attach(footer);

	auto content = std::make_shared<Scene::Node>();
	content->setStretch(1.0f);
	content->setY(header->getHeight());
	content->setVerticalMargin(header->getHeight() + footer->getHeight());
	rect->attach(content);

	auto block = std::make_shared<Shared::SceneHelpers::Backshaded<Helpers::Label>>();
	block->setAnchor(0.5f);
	block->setPivot(0.5f);
	block->getBackshadeColor()->setColor(Graphics::Color::Black);
	block->getBackshadeColor()->setAlpha(0.5f);
	block->setText("SOCIAL_WINDOW_SERVER_UNAVAILABLE");
	//rect->attach(block);

	runAction(Actions::Factory::ExecuteInfinite([block] {
	//	block->setEnabled(!CLIENT->isConnected());
	}));

	auto header_bg = std::make_shared<Scene::Rectangle>();
	header_bg->setStretch(1.0f);
	header_bg->setColor(Helpers::BaseWindowColor);
	header_bg->setAlpha(0.25f);
	header->attach(header_bg);

	auto title = std::make_shared<Helpers::Label>();
	title->setFontSize(20.0f);
	title->setText(LOCALIZE("SOCIAL_WINDOW_TITLE"));
	title->setAnchor(0.5f);
	title->setPivot(0.5f);
	header_bg->attach(title);

	auto nickname_label = std::make_shared<Helpers::Label>();
	nickname_label->setPosition({ 16.0f, 18.0f });
	nickname_label->setAnchor({ 0.0f, 0.0f });
	nickname_label->setPivot({ 0.0f, 0.5f });
	nickname_label->setText(LOCALIZE("SOCIAL_WINDOW_NICKNAME"));
	content->attach(nickname_label);

	auto nickname_field = std::make_shared<Scene::ClippableStencil<Scene::Rectangle>>();
	nickname_field->setPosition({ 128.0f, 18.0f });
	nickname_field->setSize({ 174.0f, 24.0f });
	nickname_field->setRounding(0.5f);
	nickname_field->setPivot({ 0.0f, 0.5f });
	nickname_field->setColor(Helpers::BaseWindowColor / 24.0f);
	content->attach(nickname_field);

	auto nickname_field_input = std::make_shared<Helpers::TextInputField>();
	nickname_field_input->setAnchor(0.5f);
	nickname_field_input->setPivot(0.5f);
	nickname_field_input->setStretch(1.0f);
	nickname_field_input->getLabel()->setText("guest");
	nickname_field->attach(nickname_field_input);

	auto nickname_field2 = std::make_shared<Scene::ClippableStencil<Scene::Rectangle>>();
	nickname_field2->setPosition({ 128.0f, 98.0f });
	nickname_field2->setSize({ 174.0f, 24.0f });
	nickname_field2->setRounding(0.5f);
	nickname_field2->setPivot({ 0.0f, 0.5f });
	nickname_field2->setColor(Helpers::BaseWindowColor / 24.0f);
	content->attach(nickname_field2);

	auto nickname_field_input2 = std::make_shared<Helpers::TextInputField>();
	nickname_field_input2->setAnchor(0.5f);
	nickname_field_input2->setPivot(0.5f);
	nickname_field_input2->setStretch(1.0f);
	nickname_field_input2->getLabel()->setText("guest");
	nickname_field2->attach(nickname_field_input2);
}