#include "gameplay_screen.h"

using namespace hcg001;

GameplayScreen::GameplayScreen()
{
	auto ready_label = std::make_shared<Scene::Label>();
	ready_label->setFont(FONT("default"));
	ready_label->setFontSize(28.0f);
	ready_label->setAnchor({ 0.5f, 0.25f });
	ready_label->setPivot({ 0.5f, 0.5f });
	ready_label->setText(LOCALIZE("READY_MENU_TITLE"));
	attach(ready_label);

	setChooseCallback([this, ready_label] {
		if (mReady)
			return;

		runAction(Shared::ActionHelpers::MakeSequence(
			Shared::ActionHelpers::Hide(ready_label, 0.5f),
			Shared::ActionHelpers::Kill(ready_label)
		));
		mReadyCallback();
		mReady = true;
	});
}