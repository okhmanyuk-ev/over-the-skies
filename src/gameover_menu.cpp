#include "gameover_menu.h"
#include "helpers.h"
#include "main_menu.h"
#include "yandex.h"

using namespace hcg001;

GameoverMenu::GameoverMenu(int score)
{
	auto [node, collection] = Shared::SceneHelpers::CreateNodesFromXml(std::format(R"(
		<Node stretch="1">
			<Label anchor="0.5,0.25" pivot="0.5" font_size="56" text="{}">
				<Column autosize="true" anchor="0.5,1" pivot="0.5,0" y="24">
					<RichLabel anchor="0.5,0" pivot="0.5,0" font_size="28" text="<icon=textures/crown2.png> {}"/>
					<Node height="16"/>
					<RichLabel anchor="0.5,0" pivot="0.5,0" font_size="28" text="<icon=textures/ruby.png> {}"/>
				</Column>
			</Label>
			<Label id="tap_label" anchor="0.5,0.75" pivot="0.5" font_size="24" alpha="0" text="GAMEOVER_MENU_TAP" localized="true"/>
		</Node>
	)", score, PROFILE->getHighScore(), PROFILE->getRubies()));

	getContent()->attach(node);

	auto tap_label = std::static_pointer_cast<Scene::Label>(collection.at("tap_label"));
	runAction(sky::Actions::Delayed([this] { return getState() != State::Entered; },
		sky::Actions::RepeatInfinite([tap_label] {
			return sky::Actions::Sequence(
				sky::Actions::Show(tap_label, 0.75f),
				sky::Actions::Hide(tap_label, 0.75f)
			);
		})
	));

	setTapCallback([] {
		SCENE_MANAGER->switchScreen(Helpers::gMainMenu);
	});

	Yandex::SendHighScore(score);
}
