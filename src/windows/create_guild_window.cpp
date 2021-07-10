#include "create_guild_window.h"
#include "response_wait_window.h"
#include "guilds_window.h"

using namespace hcg001;

CreateGuildWindow::CreateGuildWindow()
{
	getBackground()->setSize({ 314.0f, 152.0f });
	getTitle()->setText(LOCALIZE("CREATE_GUILD_WINDOW_TITLE"));

	auto title_label = std::make_shared<Helpers::Label>();
	title_label->setFontSize(16.0f);
	title_label->setPosition({ 16.0f, 24.0f });
	title_label->setPivot({ 0.0f, 0.5f });
	title_label->setText(LOCALIZE("TITLE"));
	getBody()->attach(title_label);

	auto title_input_field = std::make_shared<Helpers::TextInputField>();
	title_input_field->setPosition({ 128.0f, 24.0f });
	title_input_field->setSize({ 174.0f, 24.0f });
	title_input_field->setPivot({ 0.0f, 0.5f });
	title_input_field->getLabel()->setText("guild_name_here");
	getBody()->attach(title_input_field);

	auto create_button = std::make_shared<Helpers::Button>();
	create_button->getLabel()->setText(LOCALIZE("CREATE"));
	create_button->setClickCallback([title_input_field] {
		auto window = std::make_shared<ResponseWaitWindow>();
		SCENE_MANAGER->pushWindow(window);
		window->runAction(Actions::Collection::MakeSequence(
			Actions::Collection::Wait([window] { return window->getState() != Window::State::Opened; }),
			Actions::Collection::Execute([title_input_field] {
				CLIENT->createGuild(title_input_field->getLabel()->getText().cpp_str());
			}),
			Actions::Collection::Breakable(5.0f,
				Actions::Collection::WaitEvent<Channel::CreateGuildEvent>([](const auto& e) {
					LOGF("guild created with status {}", e.status);
				})
			),
			Actions::Collection::Wait(0.5f),
			Actions::Collection::Execute([] {
				SCENE_MANAGER->popWindow([] {
					if (!PROFILE->isInGuild())
						return;

					SCENE_MANAGER->popWindow(2, [] {
						auto window = std::make_shared<GuildsWindow>();
						SCENE_MANAGER->pushWindow(window);
					}); 
				});
			})
		));
	});
	create_button->setAnchor({ 0.5f, 1.0f });
	create_button->setPivot(0.5f);
	create_button->setSize({ 128.0f, 28.0f });
	create_button->setY(-24.0f);
	getBody()->attach(create_button);
}