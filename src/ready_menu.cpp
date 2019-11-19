#include "ready_menu.h"

using namespace hcg001;

ReadyMenu::ReadyMenu()
{
	auto title = std::make_shared<Scene::Label>();
	title->setFont(FONT("default"));
	title->setFontSize(28.0f);
	title->setAnchor({ 0.5f, 0.25f });
	title->setPivot({ 0.5f, 0.5f });
	title->setText(LOCALIZE("READY_MENU_TITLE"));
	attach(title);

	setChooseCallback([this] {
		if (mReadyCallback)
		{
			mReadyCallback();
		}
	});
}