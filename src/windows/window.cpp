#include "window.h"

using namespace hcg001;

Window::Window(bool has_close_button) : Shared::SceneHelpers::StandardWindow({ BackgroundEffect::Blur })
{
	mBackground = std::make_shared<Scene::ClippableStencil<Scene::Rectangle>>();
	mBackground->setSlicedSpriteOptimizationEnabled(false); // this enables nice clipping
	mBackground->setRounding(12.0f);
	mBackground->setAbsoluteRounding(true);
	mBackground->setAnchor(0.5f);
	mBackground->setPivot(0.5f);
	mBackground->setTouchable(true);
	mBackground->setColor(Helpers::Pallete::WindowBody);
	getContent()->attach(mBackground);

	auto header = std::make_shared<Scene::Node>();
	header->setHorizontalStretch(1.0f);
	header->setHeight(36.0f);
	mBackground->attach(header);

	mBody = std::make_shared<Scene::Node>();
	mBody->setStretch(1.0f);
	mBody->setY(header->getHeight());
	mBody->setVerticalMargin(header->getHeight());
	mBackground->attach(mBody);

	auto header_bg = std::make_shared<Scene::Rectangle>();
	header_bg->setStretch(1.0f);
	header_bg->setColor(Helpers::Pallete::WindowHead);
	//header_bg->setAlpha(0.25f);
	header->attach(header_bg);

	mTitle = std::make_shared<Helpers::Label>();
	mTitle->setColor(Helpers::Pallete::WindowTitle);
	mTitle->setFontSize(20.0f);
	mTitle->setAnchor(0.5f);
	mTitle->setPivot(0.5f);
	header_bg->attach(mTitle);

	if (has_close_button)
	{
		auto close_button = std::make_shared<Shared::SceneHelpers::BouncingButtonBehavior<Scene::Clickable<Scene::Adaptive<Scene::Sprite>>>>();
		close_button->setTexture(TEXTURE("textures/close2.png"));
		close_button->setAdaptSize(22.0f);
		close_button->setAnchor({ 1.0f, 0.5f });
		close_button->setPivot(0.5f);
		close_button->setX(-18.0f);
		close_button->setClickCallback([] {
			SCENE_MANAGER->popWindow();
		});
		header->attach(close_button);
	}
}