#include "create_guild_window.h"
#include "response_wait_window.h"

using namespace hcg001;

CreateGuildWindow::CreateGuildWindow()
{
	getBackground()->setSize({ 314.0f, 386.0f });
	getTitle()->setText(LOCALIZE("CREATE_GUILD_WINDOW_TITLE"));



	auto title_label = std::make_shared<Helpers::Label>();
	title_label->setFontSize(16.0f);
	title_label->setPosition({ 16.0f, 24.0f });
	title_label->setPivot({ 0.0f, 0.5f });
	title_label->setText(LOCALIZE("TITLE"));
	getBody()->attach(title_label);

	auto title_field_bg = std::make_shared<Scene::ClippableStencil<Scene::Rectangle>>();
	title_field_bg->setPosition({ 128.0f, 24.0f });
	title_field_bg->setSize({ 174.0f, 24.0f });
	title_field_bg->setRounding(0.5f);
	title_field_bg->setPivot({ 0.0f, 0.5f });
	title_field_bg->setColor(Helpers::BaseWindowColor / 24.0f);
	getBody()->attach(title_field_bg);

	auto title_input_field = std::make_shared<Helpers::TextInputField>();
	title_input_field->setAnchor(0.5f);
	title_input_field->setPivot(0.5f);
	title_input_field->setStretch(1.0f);
	title_input_field->getLabel()->setText("guild_name_here");
	title_field_bg->attach(title_input_field);

	auto create_button = std::make_shared<Helpers::Button>();
	create_button->getLabel()->setText(LOCALIZE("CREATE"));
	create_button->setClickCallback([title_input_field] {
		auto window = std::make_shared<ResponseWaitWindow>();
		SCENE_MANAGER->pushWindow(window);
		window->runAction(Actions::Factory::MakeSequence(
			Actions::Factory::Wait([window] { return window->getState() != Window::State::Opened; }),
			Actions::Factory::Execute([title_input_field] {
				CLIENT->createGuild(title_input_field->getLabel()->getText().cpp_str());
			}),
			Actions::Factory::Breakable(5.0f, 
				Actions::Factory::WaitEvent<Channel::CreateGuildEvent>([](const auto& e) {
					LOGF("guild created with status {}", e.status);
				})
			),
			Actions::Factory::Wait(0.5f),
			Actions::Factory::Execute([] {
				SCENE_MANAGER->popWindow();
			})
		));
	});
	create_button->setAnchor({ 0.5f, 1.0f });
	create_button->setPivot(0.5f);
	create_button->setSize({ 128.0f, 28.0f });
	create_button->setY(-24.0f);
	getBody()->attach(create_button);
}