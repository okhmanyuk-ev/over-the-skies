#include "gameplay_screen.h"

using namespace hcg001;

GameplayScreen::GameplayScreen()
{
	setTouchable(true);

	mReadyLabel = std::make_shared<Scene::Label>();
	mReadyLabel->setFont(FONT("default"));
	mReadyLabel->setFontSize(28.0f);
	mReadyLabel->setAnchor({ 0.5f, 0.25f });
	mReadyLabel->setPivot({ 0.5f, 0.5f });
	mReadyLabel->setText(LOCALIZE("READY_MENU_TITLE"));
	attach(mReadyLabel);
}

void GameplayScreen::touch(Touch type, const glm::vec2& pos)
{
	Scene::Actionable<Screen>::touch(type, pos);

	if (!mReady)
	{
		if (type == Touch::Begin)
		{
			runAction(Shared::ActionHelpers::MakeSequence(
				Shared::ActionHelpers::Hide(mReadyLabel, 0.5f),
				Shared::ActionHelpers::Kill(mReadyLabel)
			));
			mReadyCallback();
			mReady = true;
		}
		return;
	}
}
