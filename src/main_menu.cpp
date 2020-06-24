#include "main_menu.h"
#include "defines.h"
#include "profile.h"
#include "helpers.h"

using namespace hcg001;

MainMenu::MainMenu()
{
	auto title = std::make_shared<Scene::Label>();
	title->setFont(FONT("default"));
	title->setFontSize(34.0f);
	title->setAnchor({ 0.5f, 0.25f });
	title->setPivot(0.5f);
	title->setText(LOCALIZE("MAIN_MENU_TITLE"));
	attach(title);

	const float ButtonLabelFontSize = 20.0f;
	const glm::vec2 ButtonSize = { 192.0f, 48.0f };

	auto play_button = std::make_shared<Shared::SceneHelpers::FastButton>();
	play_button->setRounding(Helpers::ButtonRounding);
	play_button->getLabel()->setFontSize(ButtonLabelFontSize);
	play_button->getLabel()->setText(LOCALIZE("MAIN_MENU_PLAY"));
	play_button->setClickCallback([this] {
		if (PROFILE->isSkinLocked(mChoosedSkin))
			return;

		if (mStartCallback)
			mStartCallback();
	});
	play_button->setSize(ButtonSize);
	play_button->setAnchor({ -0.5f, 0.75f });
	play_button->setPivot(0.5f);
	attach(play_button);

	auto unlock_button = std::make_shared<Shared::SceneHelpers::FastButton>();
	unlock_button->setRounding(Helpers::ButtonRounding);
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
	unlock_button->setAnchor({ 1.5f, 0.75f });
	unlock_button->setPivot(0.5f);
	attach(unlock_button);

	runAction(Shared::ActionHelpers::ExecuteInfinite([this, play_button, unlock_button] {
		if (!mDecideButtons)
			return;

		if (mButtonsAnimating)
			return;
		
		auto locked = PROFILE->isSkinLocked(mChoosedSkin);

		const float Duration = 0.25f;

		if (mPlayButtonVisible && locked)
		{
			mPlayButtonVisible = false;
			mButtonsAnimating = true;
			
			runAction(Shared::ActionHelpers::MakeSequence(
				Shared::ActionHelpers::MakeParallel(
					Shared::ActionHelpers::ChangeHorizontalAnchor(play_button, -0.5f, Duration, Common::Easing::BackOut),
					Shared::ActionHelpers::ChangeHorizontalAnchor(unlock_button, 0.5f, Duration, Common::Easing::BackOut)
				),
				Shared::ActionHelpers::Execute([this] { mButtonsAnimating = false; })
			));
		}
		else if (!mPlayButtonVisible && !locked)
		{
			mPlayButtonVisible = true;
			mButtonsAnimating = true;
			runAction(Shared::ActionHelpers::MakeSequence(
				Shared::ActionHelpers::MakeParallel(
					Shared::ActionHelpers::ChangeHorizontalAnchor(play_button, 0.5f, Duration, Common::Easing::BackOut),
					Shared::ActionHelpers::ChangeHorizontalAnchor(unlock_button, 1.5f, Duration, Common::Easing::BackOut)
				),
				Shared::ActionHelpers::Execute([this] { mButtonsAnimating = false; })
			));
		}
	}));

	refresh();

	runAction(Shared::ActionHelpers::ExecuteInfinite([this] {
		if (!mScrollbox)
			return;

		if (mScrollbox->isTouching())
			return;
		
		if (glm::length(mScrollbox->getSpeed()) >= 2.0f)
			return;

		if (!mScrollbox->isTransformReady())
			return;

		auto slot_projected = mScrollbox->project(mScrollbox->getSize() / 2.0f);
		float distance = 99999.0f;
		std::shared_ptr<Scene::Node> nearest = nullptr;

		if (mScrollTarget)
		{
			nearest = mScrollTarget;
			auto nearest_projected = nearest->project(nearest->getSize() / 2.0f);
			distance = glm::distance(slot_projected, nearest_projected) / PLATFORM->getScale();

			if (distance <= SlotWidth / 2.0f)
				mScrollTarget = nullptr;
		}
		else
		{
			for (int i = 0; i < mItems.size(); i++)
			{
				auto node = mItems.at(i);
				auto node_projected = node->project(node->getSize() / 2.0f);
				auto d = glm::distance(slot_projected, node_projected) / PLATFORM->getScale();

				if (distance <= d)
					continue;

				distance = d;
				nearest = node;
				mChoosedSkin = static_cast<Skin>(i);
			}
		}

		auto nearest_projected = nearest->project(nearest->getSize() / 2.0f);
		auto offset = distance * Clock::ToSeconds(FRAME->getTimeDelta()) * 10.0f;

		if (nearest_projected.x < slot_projected.x)
			mScrollbox->getContent()->setX(mScrollbox->getContent()->getX() + offset);
		else
			mScrollbox->getContent()->setX(mScrollbox->getContent()->getX() - offset);

		mDecideButtons = distance <= 32.0f && mScrollTarget == nullptr;
	}));

	auto purchase_button = std::make_shared<Shared::SceneHelpers::FastButton>();
	purchase_button->setRounding(Helpers::ButtonRounding);
	purchase_button->getLabel()->setText(LOCALIZE("MAIN_MENU_PURCHASE_RUBIES"));
	purchase_button->getLabel()->setFontSize(14.0f);
	purchase_button->setSize({ 86.0f, 28.0f });
	purchase_button->setPosition({ 100.0f, 28.0f });
	purchase_button->setPivot({ 0.0f, 0.5f });
	purchase_button->setClickCallback([] {
		PLATFORM->purchase("rubies.001");
	});
	attach(purchase_button);
}

void MainMenu::refresh()
{
	float content_x = 0.0f;

	if (mScrollbox)
	{
		content_x = mScrollbox->getContent()->getX();
		detach(mScrollbox);
	}

	mScrollbox = std::make_shared<Scene::Scrollbox>();
	mScrollbox->setSensitivity({ 1.0f, 0.0f });
	mScrollbox->setAnchor(0.5f);
	mScrollbox->setPivot(0.5f);
	mScrollbox->setHorizontalStretch(1.0f);
	mScrollbox->setHeight(ItemSize + ScrollPadding);
	mScrollbox->setInertiaFriction(0.1f);

	mItems = createScrollItems();
	
	auto cell_size = glm::vec2({ SlotWidth, ItemSize });
	auto grid = Shared::SceneHelpers::MakeHorizontalGrid(cell_size, mItems);

	grid->setAnchor(0.5f);
	grid->setPivot(0.5f);

	mScrollbox->getContent()->setWidth(grid->getWidth() + ItemSize);
	mScrollbox->getContent()->setHeight(mScrollbox->getHeight());
	mScrollbox->getContent()->setX(content_x);
	mScrollbox->getContent()->attach(grid);

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
		auto item = std::make_shared<Scene::Clickable<Scene::Node>>();
		item->setStretch(1.0f);
		item->setPivot(0.5f);
		item->setAnchor(0.5f);
		item->setTouchMask(1 << 1);
		result.push_back(item);

		auto image = std::make_shared<Scene::Actionable<Scene::Sprite>>();
		image->setTexture(TEXTURE(path));
		image->setSampler(Renderer::Sampler::Linear);
		image->setSize({ SkinSize, SkinSize });
		image->setAnchor(0.5f);
		image->setPivot(0.5f);
		item->attach(image);

		image->runAction(Shared::ActionHelpers::ExecuteInfinite([this, image, SkinSize, SkinSizeChoosed] {
			if (!mScrollbox->isTransformReady())
				return;

			auto skin_projected = image->project(image->getSize() / 2.0f);
			auto slot_projected = mScrollbox->project(mScrollbox->getSize() / 2.0f);
			auto distance = glm::distance(skin_projected, slot_projected) / PLATFORM->getScale();
			auto size = glm::lerp(SkinSize, SkinSizeChoosed, glm::smoothstep(ItemSize, 0.0f, distance));
			image->setSize(size);
		}));

		item->setClickCallback([this, item] {
			mScrollTarget = item;
		});

		// footer

		auto footer = std::make_shared<Scene::Actionable<Scene::Node>>();
		footer->setAnchor({ 0.5f, 1.0f });
		footer->setPivot({ 0.5f, 0.0f });
		footer->setPosition({ 0.0f, 24.0f });
		image->attach(footer);

		auto hideFarNode = [this, item](auto node) {
			if (!mScrollbox->isTransformReady())
				return;
			
			auto item_projected = item->project(item->getSize() / 2.0f);
			auto slot_projected = mScrollbox->project(mScrollbox->getSize() / 2.0f);
			auto distance = glm::distance(item_projected, slot_projected) / PLATFORM->getScale();
			auto alpha = glm::smoothstep(ItemSize, ItemSize / 2.0f, distance);
			node->setAlpha(alpha);
		};

		if (PROFILE->isSkinLocked(skin))
		{
			image->setColor({ 0.5f, 0.5f, 0.5f });

			// padlock

			auto padlock = std::make_shared<Scene::Sprite>();
			padlock->setTexture(TEXTURE("textures/padlock.png"));
			padlock->setScale({ 0.04f, 0.04f });
			padlock->setAnchor({ 0.5f, 0.0f });
			padlock->setPivot({ 0.5f, 1.0f });
			padlock->setPosition({ 0.0f, -16.0f });
			image->attach(padlock);

			// price

			const float PriceRubyPadding = 6.0f;

			auto price = std::make_shared<Scene::Actionable<Scene::Label>>();
			price->setFont(FONT("default"));
			price->setFontSize(16.0f);
			price->setText(std::to_string(SkinCost.at(skin)));
			price->setAnchor({ 0.0f, 0.5f });
			price->setPivot({ 0.0f, 0.5f });
			footer->attach(price);

			price->runAction(Shared::ActionHelpers::ExecuteInfinite([hideFarNode, price] {
				hideFarNode(price);
			}));

			// ruby

			auto ruby = std::make_shared<Scene::Actionable<Scene::Sprite>>();
			ruby->setTexture(TEXTURE("textures/ruby.png"));
			ruby->setSize({ 16.0f, 16.0f });
			ruby->setAnchor({ 1.0f, 0.5f });
			ruby->setPivot({ 0.0f, 0.5f });
			ruby->setPosition({ PriceRubyPadding, 0.0f });
			price->attach(ruby);

			ruby->runAction(Shared::ActionHelpers::ExecuteInfinite([hideFarNode, ruby] {
				hideFarNode(ruby);
			}));

			footer->runAction(Shared::ActionHelpers::ExecuteInfinite([footer, price, ruby, PriceRubyPadding] {
				footer->setWidth(price->getWidth() + ruby->getWidth() + PriceRubyPadding);
			}));
		}
		else
		{
			// title

			auto name = std::make_shared<Scene::Actionable<Scene::Label>>();
			name->setFont(FONT("default"));
			name->setFontSize(16.0f);
			name->setText(LOCALIZE("SKIN_NAME_" + std::to_string((int)skin)));
			name->setAnchor({ 0.5f, 0.5f });
			name->setPivot({ 0.5f, 0.5f });
			footer->attach(name);

			name->runAction(Shared::ActionHelpers::ExecuteInfinite([hideFarNode, name] {
				hideFarNode(name);
			}));
		}
	}

	return result;
}