#include "gameover_menu.h"
#include "helpers.h"
#include "main_menu.h"
#include "yandex.h"

using namespace hcg001;

GameoverMenu::GameoverMenu(int score)
{
	auto [node, collection] = Shared::SceneHelpers::CreateNodesFromXml(std::format(R"(
		<Node stretch="full">
			<Label anchor="0.5,0.25" pivot="center" font_size="56" text="{}">
				<Column autosize="true" anchor="bottom_center" pivot="top_center" y="24">
					<RichLabel anchor="top_center" pivot="top_center" font_size="28" text="<icon=textures/crown2.png> {}"/>
					<Node height="16"/>
					<RichLabel anchor="top_center" pivot="top_center" font_size="28" text="<icon=textures/ruby.png> {}"/>
				</Column>
			</Label>
			<Label id="tap_label" anchor="0.5,0.75" pivot="center" font_size="24" alpha="0" text="GAMEOVER_MENU_TAP" localized="true"/>
		</Node>
	)", score, PROFILE->getHighScore(), PROFILE->getRubies()));

	getContent()->attach(node);

	auto tap_label = std::static_pointer_cast<Scene::Label>(collection.at("tap_label"));
	runAction(Actions::Collection::Delayed([this] { return getState() != State::Entered; },
		Actions::Collection::RepeatInfinite([tap_label] {
			return Actions::Collection::MakeSequence(
				Actions::Collection::Show(tap_label, 0.75f),
				Actions::Collection::Hide(tap_label, 0.75f)
			);
		})
	));

	setTapCallback([] {
		SCENE_MANAGER->switchScreen(Helpers::gMainMenu);
	});

	Yandex::SendHighScore(score);
}
