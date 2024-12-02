#include "achievements_window.h"
#include "achievements.h"
#include "profile.h"
#include "main_menu.h"

using namespace hcg001;

AchievementsWindow::AchievementsWindow()
{
	getBackground()->setSize({ 314.0f, 512.0f });
	getTitle()->setText(LOCALIZE("ACHIEVEMENTS_WINDOW_TITLE"));

	auto ok_button = std::make_shared<Helpers::Button>();
	ok_button->getLabel()->setText(LOCALIZE("WINDOW_OK"));
	ok_button->getLabel()->setFontSize(18.0f);
	ok_button->setClickCallback([] {
		SCENE_MANAGER->popWindow();
	});
	ok_button->setAnchor({ 0.5f, 1.0f });
	ok_button->setPivot(0.5f);
	ok_button->setSize({ 128.0f, 28.0f });
	ok_button->setY(-24.0f);
	getBody()->attach(ok_button);

	std::vector<std::shared_ptr<Scene::Node>> items;
	int num = 0;
	for (auto achievement : ACHIEVEMENTS->getItems())
	{
		num += 1;
		auto item = std::make_shared<Item>(num, achievement.name);
		items.push_back(item);
	}

	auto scrollbox = std::make_shared<Scene::ClippableScissor<Scene::Scrollbox>>();
	scrollbox->getContent()->setAutoHeightEnabled(true);
	scrollbox->getBounding()->setStretch(1.0f);
	scrollbox->getContent()->setStretch({ 1.0f, 0.0f });
	scrollbox->setStretch(1.0f);
	scrollbox->getBounding()->setAnchor(0.5f);
	scrollbox->getBounding()->setPivot(0.5f);
	scrollbox->getBounding()->setVerticalMargin(Item::VerticalMargin);
	scrollbox->setVerticalMargin(48.0f);
	getBody()->attach(scrollbox);

	auto scrollbar = std::make_shared<Shared::SceneHelpers::VerticalScrollbar>();
	scrollbar->setX(-4.0f);
	scrollbar->setScrollbox(scrollbox);
	scrollbox->attach(scrollbar);

	auto column = std::make_shared<Scene::CulledList<Scene::AutoSized<Scene::Column>>>();
	column->setAutoWidthEnabled(false);
	column->setStretch({ 1.0f, 0.0f });
	column->setCullTarget(scrollbox);

	for (auto item : items)
	{
		auto cell = std::make_shared<Scene::AutoSized<Scene::Node>>();
		cell->setAutoWidthEnabled(false);
		cell->setStretch({ 1.0f, 0.0f });
		cell->attach(item);
		column->attach(cell);
	}

	scrollbox->getContent()->attach(column);

	auto top_gradient = std::make_shared<Scene::Rectangle>();
	top_gradient->setStretch({ 1.0f, 0.0f });
	top_gradient->setHeight(Item::VerticalMargin);
	top_gradient->setAnchor({ 0.5f, 0.0f });
	top_gradient->setPivot({ 0.5f, 0.0f });
	top_gradient->getEdgeColor(Scene::Rectangle::Edge::Top)->setColor({ Helpers::Pallete::WindowBody, 1.0f });
	top_gradient->getEdgeColor(Scene::Rectangle::Edge::Bottom)->setColor({ Helpers::Pallete::WindowBody, 0.0f });
	scrollbox->attach(top_gradient);

	auto bottom_gradient = std::make_shared<Scene::Rectangle>();
	bottom_gradient->setStretch({ 1.0f, 0.0f });
	bottom_gradient->setHeight(Item::VerticalMargin);
	bottom_gradient->setAnchor({ 0.5f, 1.0f });
	bottom_gradient->setPivot({ 0.5f, 1.0f });
	bottom_gradient->getEdgeColor(Scene::Rectangle::Edge::Top)->setColor({ Helpers::Pallete::WindowBody, 0.0f });
	bottom_gradient->getEdgeColor(Scene::Rectangle::Edge::Bottom)->setColor({ Helpers::Pallete::WindowBody, 1.0f });
	scrollbox->attach(bottom_gradient);

	auto safe_area = std::make_shared<Shared::SceneHelpers::SafeArea>();
	attach(safe_area);

	RubiesIndicator = std::make_shared<Helpers::RubiesIndicator>();
	RubiesIndicator->setInstantRefresh(false);
	RubiesIndicator->makeHidden();
	safe_area->attach(RubiesIndicator);
}

AchievementsWindow::~AchievementsWindow()
{
	RubiesIndicator = nullptr;
}

void AchievementsWindow::onOpenEnd()
{
	Window::onOpenEnd();
	RubiesIndicator->show();
	Helpers::gMainMenu->getRubiesIndicator()->hide();
}

void AchievementsWindow::onCloseBegin()
{
	Window::onCloseBegin();
	RubiesIndicator->hide();
	Helpers::gMainMenu->getRubiesIndicator()->show();
}

AchievementsWindow::Item::Item(int num, const std::string& name) : mName(name)
{
	setStretch({ 1.0f, 0.0f });
	setHeight(64.0f);

	auto rect = std::make_shared<Scene::Rectangle>();
	rect->setStretch(1.0f);
	rect->setAnchor(0.5f);
	rect->setPivot(0.5f);
	rect->setRounding(8.0f);
	rect->setAbsoluteRounding(true);
	rect->setColor(Helpers::Pallete::WindowItem);
	rect->setMargin({ 16.0f, VerticalMargin });
	rect->setBatchGroup("achievement_item");
	attach(rect);

	auto num_label = std::make_shared<Helpers::Label>();
	num_label->setFontSize(24.0f);
	num_label->setPosition({ 24.0f, 0.0f });
	num_label->setPivot(0.5f);
	num_label->setAnchor({ 0.0f, 0.5f });
	num_label->setText(std::to_wstring(num));
	rect->attach(num_label);

	auto title_holder = std::make_shared<Scene::AutoSized<Scene::Node>>();
	title_holder->setWidth(164.0f);
	title_holder->setPosition({ 48.0f, 8.0f });
	title_holder->setAutoWidthEnabled(false);
	rect->attach(title_holder);

	auto title = std::make_shared<Scene::AutoScaled<Helpers::Label>>();
	title->setAnchor({ 0.0f, 0.5f });
	title->setPivot({ 0.0f, 0.5f });
	title->setFontSize(16.0f);
	title->setText(LOCALIZE("ACHIEVEMENT_" + name));
	title->setColor(Helpers::Pallete::YellowLabel);
	title_holder->attach(title);

	auto progress = ACHIEVEMENTS->getProgress(name);
	auto required = ACHIEVEMENTS->getRequired(name);
	auto achievement = ACHIEVEMENTS->getItemByName(name).value();
	bool completed = progress >= required;

	auto progress_label = std::make_shared<Helpers::Label>();
	progress_label->setPosition({ 48.0f, 28.0f });
	progress_label->setFontSize(12.0f);
	progress_label->setText(fmt::format(L"{}/{}", progress, required));
	rect->attach(progress_label);

	auto progressbar = std::make_shared<Shared::SceneHelpers::Progressbar>();
	progressbar->setSize({ 148.0f, 4.0f });
	progressbar->setPosition({ 48.0f, 42.0f });
	progressbar->setProgress((float)progress / (float)required);
	progressbar->setBatchGroup("achievement_progressbar");
	progressbar->getProgressContent()->setBatchGroup("achievement_progressbar_content");
	rect->attach(progressbar);

	auto ruby = std::make_shared<Scene::Adaptive<Scene::Sprite>>();
	ruby->setTexture(TEXTURE("textures/ruby.png"));
	ruby->setAnchor({ 0.0f, 0.5f });
	ruby->setPivot({ 1.0f, 0.5f });
	ruby->setX(-3.0f);
	ruby->setAdaptSize(12.0f);
	ruby->setBatchGroup("achievement_ruby");

	mButtonHolder = std::make_shared<Scene::Node>();
	mButtonHolder->setAnchor({ 1.0f, 0.5f });
	mButtonHolder->setPivot({ 1.0f, 0.5f });
	mButtonHolder->setX(-44.0f);
	rect->attach(mButtonHolder);

	mButton = std::make_shared<Helpers::Button>();
	mButton->setTouchMask(1 << 1);
	mButton->setActive(completed);
	mButton->setButtonColor(glm::rgbColor(glm::vec3(150.0f, 0.5f, 0.5f)));
	//mButton->getLabel()->setText(LOCALIZE("ACHIEVEMENTS_WINDOW_CLAIM"));
	mButton->getLabel()->setText(std::to_wstring(achievement.reward));
	mButton->setClickCallback([this, name, ruby, achievement] {
		PROFILE->achievementRewardTake(name);
		PROFILE->increaseRubies(achievement.reward);
		PROFILE->saveAsync();

		ruby->setBatchGroup("");
		ruby->setAdaptingEnabled(false);
		ruby->bakeAdaption();
		RubiesIndicator->collectRubyAnim(ruby);
		refresh(true);
	});
	mButton->setAnchor(0.5f);
	mButton->setPivot(0.5f);
	mButton->setSize({ 72.0f, 24.0f });
	mButton->getLabel()->setPivot({ 0.0f, 0.5f });
	mButton->setBatchGroup("achievement_button");
	mButtonHolder->attach(mButton);

	mButton->getLabel()->attach(ruby);

	mCheck = std::make_shared<Scene::Adaptive<Scene::Sprite>>();
	mCheck->setTexture(TEXTURE("textures/check.png"));
	mCheck->setAnchor(0.5f);
	mCheck->setPivot(0.5f);
	mCheck->setAdaptSize(32.0f);
	mCheck->setBatchGroup("achievement_check");
	mButtonHolder->attach(mCheck);

	refresh();
}

void AchievementsWindow::Item::refresh(bool anim)
{
	bool reward_taken = PROFILE->isAchievementRewardTaken(mName);

	if (!anim)
	{
		mButton->setEnabled(!reward_taken);
		mCheck->setEnabled(reward_taken);
		return;
	}

	if (mButton->isEnabled() && reward_taken)
	{
		runAction(Actions::Collection::MakeSequence(
			Actions::Collection::ChangeScale(mButtonHolder, { 0.0f, 0.0f }, 0.25f, Easing::CubicOut),
			Actions::Collection::Execute([this, reward_taken] {
				mButton->setEnabled(false);
				mCheck->setEnabled(true);
			}),
			Actions::Collection::ChangeScale(mButtonHolder, { 1.0f, 1.0f }, 0.25f, Easing::BackOut)
		));
	}
}