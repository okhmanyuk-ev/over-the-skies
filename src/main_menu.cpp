#include "main_menu.h"
#include "profile.h"
#include "helpers.h"
#include "social_panel.h"
#include "windows/shop_window.h"
#include "windows/options_window.h"
#include "windows/achievements_window.h"
#include "windows/guilds_window.h"
#include "windows/global_chat_window.h"

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
	getContent()->attach(title);

	auto social_panel = std::make_shared<SocialPanel>();
	social_panel->setAnchor(0.5f);
	social_panel->setPivot(0.5f);
	social_panel->setPosition({ 0.0f, -96.0f + 16.0f });
	getContent()->attach(social_panel);

	const glm::vec2 ButtonSize = { 192.0f, 48.0f };

	auto play_button = std::make_shared<Helpers::Button>();	
	play_button->setActiveColor({ 1.0f, 1.0f, 1.0f, 0.33f });
	play_button->setAdaptiveFontSize(false);
	play_button->getLabel()->setFontSize(20.0f);
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
	play_button->refresh();
	getContent()->attach(play_button);

	auto unlock_button = std::make_shared<Helpers::Button>();
	unlock_button->setActiveColor({ 1.0f, 1.0f, 1.0f, 0.33f });
	unlock_button->setAdaptiveFontSize(false);
	unlock_button->getLabel()->setFontSize(20.0f);
	unlock_button->getLabel()->setText(LOCALIZE("MAIN_MENU_UNLOCK"));
	unlock_button->setClickCallback([this, unlock_button] {
		if (!PROFILE->isSkinLocked(mChoosedSkin))
			return;

		if (PROFILE->getRubies() < SkinCost.at(mChoosedSkin))
		{
			auto label = Shared::SceneHelpers::MakeFastPopupLabel(shared_from_this(), unlock_button, LOCALIZE("MAIN_MENU_NOT_ENOUG_RUBIES"), 20.0f);
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
	unlock_button->refresh();

	getContent()->attach(unlock_button);

	runAction(Actions::Collection::ExecuteInfinite([this, play_button, unlock_button] {
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
			
			runAction(Actions::Collection::MakeSequence(
				Actions::Collection::MakeParallel(
					Actions::Collection::ChangeHorizontalAnchor(play_button, -0.5f, Duration, EasingFunction),
					Actions::Collection::ChangeHorizontalAnchor(unlock_button, 0.5f, Duration, EasingFunction)
				),
				Actions::Collection::Execute([this] { mButtonsAnimating = false; })
			));
		}
		else if (!mPlayButtonVisible && !locked)
		{
			mPlayButtonVisible = true;
			mButtonsAnimating = true;
			runAction(Actions::Collection::MakeSequence(
				Actions::Collection::MakeParallel(
					Actions::Collection::ChangeHorizontalAnchor(play_button, 0.5f, Duration, EasingFunction),
					Actions::Collection::ChangeHorizontalAnchor(unlock_button, 1.5f, Duration, EasingFunction)
				),
				Actions::Collection::Execute([this] { mButtonsAnimating = false; })
			));
		}
	}));

	refresh();

	runAction(Actions::Collection::ExecuteInfinite([this](auto delta) {
		menuPhysics(Clock::ToSeconds(delta));
	}));

	const glm::vec2 TopButtonSize = { 72.0f, 28.0f };

	auto shop_button = std::make_shared<Shared::SceneHelpers::BouncingButtonBehavior<Scene::Adaptive<Scene::Clickable<Scene::Sprite>>>>();
	shop_button->setTexture(TEXTURE("textures/shop.png"));
	shop_button->setAdaptSize(TopButtonSize);
	shop_button->setPosition({ 108.0f, 24.0f });
	shop_button->setPivot({ 0.0f, 0.5f });
	shop_button->setClickCallback([] {
		auto window = std::make_shared<ShopWindow>();
		SCENE_MANAGER->pushWindow(window);
	});
    getGui()->attach(shop_button);

	auto options_button = std::make_shared<Shared::SceneHelpers::BouncingButtonBehavior<Scene::Adaptive<Scene::Clickable<Scene::Sprite>>>>();
	options_button->setTexture(TEXTURE("textures/options.png"));
	options_button->setAdaptSize({ 24.0f, 24.0f });
	options_button->setPosition({ -16.0f, 24.0f });
	options_button->setAnchor({ 1.0f, 0.0f });
	options_button->setPivot({ 1.0f, 0.5f });
	options_button->setClickCallback([] {
		auto window = std::make_shared<OptionsWindow>();
		SCENE_MANAGER->pushWindow(window);
	});
	getGui()->attach(options_button);

	auto achievements_button = std::make_shared<Shared::SceneHelpers::BouncingButtonBehavior<Scene::Adaptive<Scene::Clickable<Scene::Sprite>>>>();
	achievements_button->setTexture(TEXTURE("textures/cup.png"));
	achievements_button->setAdaptSize(TopButtonSize);
	achievements_button->setPosition({ 168.0f, 24.0f });
	achievements_button->setPivot({ 0.0f, 0.5f });
	achievements_button->setClickCallback([] {
		auto window = std::make_shared<AchievementsWindow>();
		SCENE_MANAGER->pushWindow(window);
	});
	getGui()->attach(achievements_button);

	auto guilds_button = std::make_shared<Shared::SceneHelpers::BouncingButtonBehavior<Scene::Adaptive<Scene::Clickable<Scene::Sprite>>>>();
	guilds_button->setTexture(TEXTURE("textures/guilds.png"));
	guilds_button->setAdaptSize(TopButtonSize);
	guilds_button->setPosition({ 216.0f, 24.0f });
	guilds_button->setPivot({ 0.0f, 0.5f });
	guilds_button->setClickCallback([] {
		auto window = std::make_shared<GuildsWindow>();
		SCENE_MANAGER->pushWindow(window);
	});
	getGui()->attach(guilds_button);

	auto global_chat_button = std::make_shared<Shared::SceneHelpers::BouncingButtonBehavior<Scene::Adaptive<Scene::Clickable<Scene::Sprite>>>>();
	global_chat_button->setTexture(TEXTURE("textures/chat.png"));
	global_chat_button->setAdaptSize(TopButtonSize);
	global_chat_button->setPosition({ 276.0f, 24.0f });
	global_chat_button->setPivot({ 0.0f, 0.5f });
	global_chat_button->setClickCallback([] {
		auto window = std::make_shared<GlobalChatWindow>();
		SCENE_MANAGER->pushWindow(window);
	});
	getGui()->attach(global_chat_button);

	auto rubies = std::make_shared<Helpers::RubiesIndicator>();
	getGui()->attach(rubies);
}

void MainMenu::refresh()
{
	glm::vec2 prev_scroll_pos = { 0.0f, 0.0f };

	if (mScrollbox)
	{
		prev_scroll_pos = mScrollbox->getScrollPosition();
		getContent()->detach(mScrollbox);
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

	getContent()->attach(mScrollbox);
}

std::vector<std::shared_ptr<Scene::Node>> MainMenu::createScrollItems()
{
	const float SkinSize = 24.0f;
	const float SkinSizeChoosed = 42.0f;

	std::vector<std::shared_ptr<Scene::Node>> result;

	for (const auto& [skin, path] : SkinPath)
	{
		auto locked = PROFILE->isSkinLocked(skin);

		auto item = std::make_shared<Scene::Cullable<Scene::Clickable<Scene::Node>>>();
		item->setStretch(1.0f);
		item->setPivot(0.5f);
		item->setAnchor(0.5f);
		item->setTouchMask(1 << 1);
		item->setClickCallback([this, item] {
			mScrollTarget = item;
		});
		result.push_back(item);

		auto image = std::make_shared<Scene::Sprite>();
		image->setBatchGroup("main_menu_item_image");
		image->setTexture(TEXTURE(path));
		image->setSampler(Renderer::Sampler::Linear);
		image->setSize(SkinSize);
		image->setAnchor(0.5f);
		image->setPivot(0.5f);
		item->attach(image);

		if (locked)
			image->setColor({ 0.5f, 0.5f, 0.5f });

		auto padlock = std::make_shared<Scene::Sprite>();
		padlock->setBatchGroup("main_menu_item_padlock");
		padlock->setTexture(TEXTURE("textures/padlock.png"));
		padlock->setScale({ 0.04f, 0.04f });
		padlock->setAnchor({ 0.5f, 0.0f });
		padlock->setPivot({ 0.5f, 1.0f });
		padlock->setPosition({ 0.0f, -16.0f });
		padlock->setEnabled(locked);
		image->attach(padlock);

		auto footer = std::make_shared<Scene::Node>();
		footer->setAnchor({ 0.5f, 1.0f });
		footer->setPivot({ 0.5f, 0.0f });
		footer->setPosition({ 0.0f, 24.0f });
		image->attach(footer);

		auto price = std::make_shared<Scene::Label>();
		price->setFont(FONT("default"));
		price->setFontSize(16.0f);
		price->setText(std::to_string(SkinCost.at(skin)));
		price->setAnchor({ 0.0f, 0.5f });
		price->setPivot({ 0.0f, 0.5f });
		price->setEnabled(locked);
		footer->attach(price);

		const float PriceRubyPadding = 6.0f;

		auto ruby = std::make_shared<Scene::Sprite>();
		ruby->setTexture(TEXTURE("textures/ruby.png"));
		ruby->setSize(16.0f);
		ruby->setAnchor({ 1.0f, 0.5f });
		ruby->setPivot({ 0.0f, 0.5f });
		ruby->setPosition({ PriceRubyPadding, 0.0f });
		ruby->setEnabled(locked);
		price->attach(ruby);

		auto name = std::make_shared<Scene::Label>();
		name->setFont(FONT("default"));
		name->setFontSize(16.0f);
		name->setText(LOCALIZE("SKIN_NAME_" + std::to_string((int)skin)));
		name->setAnchor(0.5f);
		name->setPivot(0.5f);
		name->setEnabled(!locked);
		footer->attach(name);

		auto hideFarNode = [this, item](auto node) {
			if (!mScrollbox->isTransformReady())
				return;

			auto item_projected = unproject(item->project(item->getAbsoluteSize() / 2.0f));
			auto slot_projected = unproject(mScrollbox->project(mScrollbox->getAbsoluteSize() / 2.0f));
			auto distance = glm::distance(item_projected, slot_projected);
			auto alpha = glm::smoothstep(ItemSize, ItemSize / 2.0f, distance);
			node->setAlpha(alpha);
		};

		image->runAction(Actions::Collection::ExecuteInfinite([this, image, SkinSize, SkinSizeChoosed, hideFarNode, footer, price, ruby, name, PriceRubyPadding, locked] {
			if (!mScrollbox->isTransformReady())
				return;

			auto skin_projected = unproject(image->project(image->getAbsoluteSize() / 2.0f));
			auto slot_projected = unproject(mScrollbox->project(mScrollbox->getAbsoluteSize() / 2.0f));
			auto distance = glm::distance(skin_projected, slot_projected);
			auto size = glm::lerp(SkinSize, SkinSizeChoosed, glm::smoothstep(ItemSize, 0.0f, distance));
			image->setSize(size);
			//auto scale = glm::lerp(1.0f, 1.75f, glm::smoothstep(ItemSize, 0.0f, distance));
			//image->setScale(scale);

			if (locked)
				footer->setWidth(price->getWidth() + ruby->getWidth() + PriceRubyPadding);

			hideFarNode(ruby);
			hideFarNode(price);
			hideFarNode(name);
		}));

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
	auto offset = distance * dTime * 10.0f / mScrollbox->getHorizontalScrollSpace();
	
	if (nearest_projected.x < slot_projected.x)
		mScrollbox->setHorizontalScrollPosition(mScrollbox->getHorizontalScrollPosition() - offset);
	else
		mScrollbox->setHorizontalScrollPosition(mScrollbox->getHorizontalScrollPosition() + offset);

	mDecideButtons = distance <= 32.0f && mScrollTarget == nullptr;
}
