#include "main_menu.h"
#include "profile.h"
#include "helpers.h"
#include "social_panel.h"
#include "windows/shop_window.h"
#include "windows/options_window.h"
#include "windows/highscores_window.h"
#include "windows/guilds_window.h"

using namespace hcg001;

MainMenu::MainMenu()
{
	auto title = std::make_shared<Scene::Label>();
	title->setFont(FONT("default"));
	title->setFontSize(34.0f);
	title->setAnchor({ 0.5f, 0.25f });
	title->setPivot(0.5f);
	title->setPosition({ 0.0f, -32.0f });
	title->setText(LOCALIZE("MAIN_MENU_TITLE"));
	attach(title);

	const float ButtonLabelFontSize = 20.0f;
	const glm::vec2 ButtonSize = { 192.0f, 48.0f };

	auto play_button = std::make_shared<Helpers::RectangleButton>();
	play_button->getLabel()->setFontSize(ButtonLabelFontSize);
	play_button->getLabel()->setText(LOCALIZE("MAIN_MENU_PLAY"));
	play_button->setClickCallback([this] {
		if (PROFILE->isSkinLocked(mChoosedSkin))
			return;

		PROFILE->setCurrentSkin(mChoosedSkin);

		if (mStartCallback)
			mStartCallback();
	});
	play_button->setSize(ButtonSize);
	play_button->setAnchor({ -0.5f, 0.5f });
	play_button->setPivot(0.5f);
	play_button->setPosition({ 0.0f, 224.0f - 16.0f });
	attach(play_button);

	auto unlock_button = std::make_shared<Helpers::RectangleButton>();
	unlock_button->getLabel()->setFontSize(ButtonLabelFontSize);
	unlock_button->getLabel()->setText(LOCALIZE("MAIN_MENU_UNLOCK"));
	unlock_button->setClickCallback([this, ButtonLabelFontSize, unlock_button] {
		if (!PROFILE->isSkinLocked(mChoosedSkin))
			return;

		if (PROFILE->getRubies() < SkinCost.at(mChoosedSkin))
		{
			auto label = Shared::SceneHelpers::MakeFastPopupLabel(shared_from_this(), unlock_button, LOCALIZE("MAIN_MENU_NOT_ENOUG_RUBIES"), ButtonLabelFontSize);
			label->setOrigin({ 0.0f, unlock_button->getHeight() });
			return;
		}

		auto buy_skin_menu = std::make_shared<BuySkinMenu>(mChoosedSkin);
		buy_skin_menu->setExitCallback([this] {
			refresh();
			SCENE_MANAGER->switchScreen(shared_from_this());
		});
		SCENE_MANAGER->switchScreen(buy_skin_menu);
	});
	unlock_button->setSize(ButtonSize);
	unlock_button->setAnchor({ 1.5f, 0.5f });
	unlock_button->setPivot(0.5f);
	unlock_button->setPosition({ 0.0f, 224.0f - 16.0f });

	attach(unlock_button);

	runAction(Actions::Factory::ExecuteInfinite([this, play_button, unlock_button] {
		if (!mDecideButtons)
			return;

		if (mButtonsAnimating)
			return;
		
		auto locked = PROFILE->isSkinLocked(mChoosedSkin);

		const float Duration = 0.33f;
		const auto EasingFunction = Easing::ExponentialOut;

		if (mPlayButtonVisible && locked)
		{
			mPlayButtonVisible = false;
			mButtonsAnimating = true;
			
			runAction(Actions::Factory::MakeSequence(
				Actions::Factory::MakeParallel(
					Actions::Factory::ChangeHorizontalAnchor(play_button, -0.5f, Duration, EasingFunction),
					Actions::Factory::ChangeHorizontalAnchor(unlock_button, 0.5f, Duration, EasingFunction)
				),
				Actions::Factory::Execute([this] { mButtonsAnimating = false; })
			));
		}
		else if (!mPlayButtonVisible && !locked)
		{
			mPlayButtonVisible = true;
			mButtonsAnimating = true;
			runAction(Actions::Factory::MakeSequence(
				Actions::Factory::MakeParallel(
					Actions::Factory::ChangeHorizontalAnchor(play_button, 0.5f, Duration, EasingFunction),
					Actions::Factory::ChangeHorizontalAnchor(unlock_button, 1.5f, Duration, EasingFunction)
				),
				Actions::Factory::Execute([this] { mButtonsAnimating = false; })
			));
		}
	}));

	refresh();

	runAction(Actions::Factory::ExecuteInfinite([this] {
		menuPhysics(Clock::ToSeconds(FRAME->getTimeDelta()));
	}));

	auto hud = std::make_shared<Shared::SceneHelpers::Hud>();
	attach(hud);

	auto shop_button = std::make_shared<Shared::SceneHelpers::BouncingButtonBehavior<Shared::SceneHelpers::Adaptive<Scene::Clickable<Scene::Sprite>>>>();
	shop_button->setTexture(TEXTURE("textures/shop.png"));
	shop_button->setAdaptSize({ 86.0f, 28.0f });
	shop_button->setPosition({ 108.0f, 24.0f });
	shop_button->setPivot({ 0.0f, 0.5f });
	shop_button->setClickCallback([] {
		auto window = std::make_shared<ShopWindow>();
		SCENE_MANAGER->pushWindow(window);
	});
	hud->attach(shop_button);

	auto options_button = std::make_shared<Shared::SceneHelpers::BouncingButtonBehavior<Shared::SceneHelpers::Adaptive<Scene::Clickable<Scene::Sprite>>>>();
	options_button->setTexture(TEXTURE("textures/options.png"));
	options_button->setAdaptSize({ 24.0f, 24.0f });
	options_button->setPosition({ -16.0f, 24.0f });
	options_button->setAnchor({ 1.0f, 0.0f });
	options_button->setPivot({ 1.0f, 0.5f });
	options_button->setClickCallback([] {
		auto window = std::make_shared<OptionsWindow>();
		SCENE_MANAGER->pushWindow(window);
	});
	hud->attach(options_button);

	auto hishscores_button = std::make_shared<Shared::SceneHelpers::BouncingButtonBehavior<Shared::SceneHelpers::Adaptive<Scene::Clickable<Scene::Sprite>>>>();
	hishscores_button->setTexture(TEXTURE("textures/podium.png"));
	hishscores_button->setAdaptSize({ 86.0f, 28.0f });
	hishscores_button->setPosition({ 164.0f, 24.0f });
	hishscores_button->setPivot({ 0.0f, 0.5f });
	hishscores_button->setClickCallback([] {
		auto window = std::make_shared<HighscoresWindow>();
		SCENE_MANAGER->pushWindow(window);
	});
	hud->attach(hishscores_button);

	auto guilds_button = std::make_shared<Shared::SceneHelpers::BouncingButtonBehavior<Shared::SceneHelpers::Adaptive<Scene::Clickable<Scene::Sprite>>>>();
	guilds_button->setTexture(TEXTURE("textures/guilds.png"));
	guilds_button->setAdaptSize({ 86.0f, 28.0f });
	guilds_button->setPosition({ 220.0f, 24.0f });
	guilds_button->setPivot({ 0.0f, 0.5f });
	guilds_button->setClickCallback([] {
		auto window = std::make_shared<GuildsWindow>();
		SCENE_MANAGER->pushWindow(window);
	});
	hud->attach(guilds_button);

	auto social_panel = std::make_shared<SocialPanel>();
	social_panel->setAnchor(0.5f);
	social_panel->setPivot(0.5f);
	social_panel->setPosition({ 0.0f, -48.0f - 16.0f });
	attach(social_panel);
}

void MainMenu::refresh()
{
	glm::vec2 prev_scroll_pos = { 0.0f, 0.0f };

	if (mScrollbox)
	{
		prev_scroll_pos = mScrollbox->getScrollPosition();
		detach(mScrollbox);
	}

	mScrollbox = std::make_shared<Scene::Scrollbox>();
	mScrollbox->setSensitivity({ 1.0f, 0.0f });
	mScrollbox->setAnchor(0.5f);
	mScrollbox->setPivot(0.5f);
	mScrollbox->setPosition({ 0.0f, 84.0f - 16.0f });
	mScrollbox->setHorizontalStretch(1.0f);
	mScrollbox->setHeight(ItemSize + ScrollPadding);
	mScrollbox->setInertiaFriction(0.1f);
	mScrollbox->setScrollPosition(prev_scroll_pos);

	mItems = createScrollItems();
	
	auto row = Shared::SceneHelpers::MakeHorizontalGrid({ SlotWidth, ItemSize }, mItems);
	row->setAnchor(0.5f);
	row->setPivot(0.5f);

	auto content = mScrollbox->getContent();

	content->setWidth(row->getWidth() + ItemSize);
	content->setHeight(mScrollbox->getHeight());
	content->attach(row);

	auto bounding = mScrollbox->getBounding();

	bounding->setAnchor(0.5f);
	bounding->setPivot(0.5f);
	bounding->setVerticalStretch(1.0f);
	bounding->setWidth(SlotWidth);

	attach(mScrollbox);
}

std::vector<std::shared_ptr<Scene::Node>> MainMenu::createScrollItems()
{
	const float SkinSize = 24.0f;
	const float SkinSizeChoosed = 42.0f;

	std::vector<std::shared_ptr<Scene::Node>> result;

	for (const auto& [skin, path] : SkinPath)
	{
		auto item = std::make_shared<Scene::Cullable<Scene::Clickable<Scene::Node>>>();
		item->setStretch(1.0f);
		item->setPivot(0.5f);
		item->setAnchor(0.5f);
		item->setTouchMask(1 << 1);
		result.push_back(item);

		auto image = std::make_shared<Scene::Sprite>();
		image->setBatchGroup("main_menu_item_image");
		image->setTexture(TEXTURE(path));
		image->setSampler(Renderer::Sampler::Linear);
		image->setSize({ SkinSize, SkinSize });
		image->setAnchor(0.5f);
		image->setPivot(0.5f);
		item->attach(image);

		image->runAction(Actions::Factory::ExecuteInfinite([this, image, SkinSize, SkinSizeChoosed] {
			if (!mScrollbox->isTransformReady())
				return;

			auto skin_projected = unproject(image->project(image->getAbsoluteSize() / 2.0f));
			auto slot_projected = unproject(mScrollbox->project(mScrollbox->getAbsoluteSize() / 2.0f));
			auto distance = glm::distance(skin_projected, slot_projected);
			auto size = glm::lerp(SkinSize, SkinSizeChoosed, glm::smoothstep(ItemSize, 0.0f, distance));
			image->setSize(size);
		}));

		item->setClickCallback([this, item] {
			mScrollTarget = item;
		});

		// footer

		auto footer = std::make_shared<Scene::Node>();
		footer->setAnchor({ 0.5f, 1.0f });
		footer->setPivot({ 0.5f, 0.0f });
		footer->setPosition({ 0.0f, 24.0f });
		image->attach(footer);

		auto hideFarNode = [this, item](auto node) {
			if (!mScrollbox->isTransformReady())
				return;
			
			auto item_projected = unproject(item->project(item->getAbsoluteSize() / 2.0f));
			auto slot_projected = unproject(mScrollbox->project(mScrollbox->getAbsoluteSize() / 2.0f));
			auto distance = glm::distance(item_projected, slot_projected);
			auto alpha = glm::smoothstep(ItemSize, ItemSize / 2.0f, distance);
			node->setAlpha(alpha);
		};

		if (PROFILE->isSkinLocked(skin))
		{
			image->setColor({ 0.5f, 0.5f, 0.5f });

			// padlock

			auto padlock = std::make_shared<Scene::Sprite>();
			padlock->setBatchGroup("main_menu_item_padlock");
			padlock->setTexture(TEXTURE("textures/padlock.png"));
			padlock->setScale({ 0.04f, 0.04f });
			padlock->setAnchor({ 0.5f, 0.0f });
			padlock->setPivot({ 0.5f, 1.0f });
			padlock->setPosition({ 0.0f, -16.0f });
			image->attach(padlock);

			// price

			const float PriceRubyPadding = 6.0f;

			auto price = std::make_shared<Scene::Label>();
			price->setFont(FONT("default"));
			price->setFontSize(16.0f);
			price->setText(std::to_string(SkinCost.at(skin)));
			price->setAnchor({ 0.0f, 0.5f });
			price->setPivot({ 0.0f, 0.5f });
			footer->attach(price);

			price->runAction(Actions::Factory::ExecuteInfinite([hideFarNode, price] {
				hideFarNode(price);
			}));

			// ruby

			auto ruby = std::make_shared<Scene::Sprite>();
			ruby->setTexture(TEXTURE("textures/ruby.png"));
			ruby->setSize({ 16.0f, 16.0f });
			ruby->setAnchor({ 1.0f, 0.5f });
			ruby->setPivot({ 0.0f, 0.5f });
			ruby->setPosition({ PriceRubyPadding, 0.0f });
			price->attach(ruby);

			ruby->runAction(Actions::Factory::ExecuteInfinite([hideFarNode, ruby] {
				hideFarNode(ruby);
			}));

			footer->runAction(Actions::Factory::ExecuteInfinite([footer, price, ruby, PriceRubyPadding] {
				footer->setWidth(price->getWidth() + ruby->getWidth() + PriceRubyPadding);
			}));
		}
		else
		{
			// title

			auto name = std::make_shared<Scene::Label>();
			name->setFont(FONT("default"));
			name->setFontSize(16.0f);
			name->setText(LOCALIZE("SKIN_NAME_" + std::to_string((int)skin)));
			name->setAnchor({ 0.5f, 0.5f });
			name->setPivot({ 0.5f, 0.5f });
			footer->attach(name);

			name->runAction(Actions::Factory::ExecuteInfinite([hideFarNode, name] {
				hideFarNode(name);
			}));
		}

		if (PROFILE->getCurrentSkin() == skin)
			mScrollTarget = item;
	}

	return result;
}

void MainMenu::menuPhysics(float dTime)
{
	if (!mScrollbox)
		return;

	if (mScrollbox->isTouching())
		return;
		
	if (glm::length(mScrollbox->getSpeed()) >= 2.0f)
		return;

	if (!mScrollbox->isTransformReady())
		return;

	auto slot_projected = unproject(mScrollbox->project(mScrollbox->getAbsoluteSize() / 2.0f));
	float distance = 99999.0f;
	std::shared_ptr<Scene::Node> nearest = nullptr;

	if (mScrollTarget)
	{
		nearest = mScrollTarget;
		auto nearest_projected = unproject(nearest->project(nearest->getAbsoluteSize() / 2.0f));
		distance = glm::distance(slot_projected, nearest_projected);

		if (distance <= SlotWidth / 2.0f)
			mScrollTarget = nullptr;
	}
	else
	{
		for (int i = 0; i < mItems.size(); i++)
		{
			auto node = mItems.at(i);
			auto node_projected = unproject(node->project(node->getAbsoluteSize() / 2.0f));
			auto d = glm::distance(slot_projected, node_projected);

			if (distance <= d)
				continue;

			distance = d;
			nearest = node;
			mChoosedSkin = static_cast<Skin>(i);
		}
	}

	auto nearest_projected = unproject(nearest->project(nearest->getAbsoluteSize() / 2.0f));
	auto offset = distance * dTime * 10.0f / mScrollbox->getHorizontalScrollSpaceSize();
	
	if (nearest_projected.x < slot_projected.x)
		mScrollbox->setHorizontalScrollPosition(mScrollbox->getHorizontalScrollPosition() - offset);
	else
		mScrollbox->setHorizontalScrollPosition(mScrollbox->getHorizontalScrollPosition() + offset);

	mDecideButtons = distance <= 32.0f && mScrollTarget == nullptr;
}
