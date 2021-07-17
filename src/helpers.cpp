#include "helpers.h"
#include "windows/input_window.h"

using namespace hcg001::Helpers;

Label::Label()
{
	setFont(FONT("default"));
	setFontSize(18.0f);
}

Button::Button()
{
	setRounding(0.5f);
	setHighlightEnabled(false);
	runAction(Actions::Collection::ExecuteInfinite([this] {
		if (!mAdaptiveFontSize)
			return;

		getLabel()->setFontSize(getAbsoluteHeight() * (18.0f / 28.0f));
	}));
	setButtonColor(Pallete::ButtonColor);
	refresh();
}

void Button::setButtonColor(const glm::vec3& color)
{
	setActiveColor({ color, 1.0f });
	setInactiveColor({ glm::rgbColor(glm::vec3(0.0f, 0.0f, 0.25f)), 1.0f });
	setHighlightColor({ color * 1.5f, 1.0f });
	refresh();
}

TextInputField::TextInputField(const utf8_string& input_window_title)
{
	setRounding(0.5f);
	setColor(Pallete::InputField);

	setClickCallback([this, input_window_title] {
		auto text = mLabel->getText();
		auto callback = [this](auto text) {
			mLabel->setText(text);
		};
		auto input_window = std::make_shared<InputWindow>(input_window_title, text, callback);
		SCENE_MANAGER->pushWindow(input_window);
	});

	mLabel = std::make_shared<Label>();
	mLabel->setAnchor(0.5f);
	mLabel->setPivot(0.5f);
	attach(mLabel);
}

float hcg001::Helpers::SmoothValueSetup(float src, float dst, Clock::Duration dTime)
{
	auto delta = dst - src;
	auto _dTime = Clock::ToSeconds(dTime);
	const float speed = 10.0f;
	return src + (delta * _dTime * speed);
}

WaitingIndicator::WaitingIndicator()
{
	setSize(24.0f);

	auto circle = std::make_shared<Scene::Circle>();
	circle->setStretch(1.0f);
	circle->setAnchor(0.5f);
	circle->setPivot(0.5f);
	circle->setFill(0.2f);
	circle->setPie(0.66f);
	attach(circle);

	runAction(Actions::Collection::RepeatInfinite([circle] {
		return Actions::Collection::ChangeRadialAnchor(circle, 0.0f, 1.0f, 1.0f);
		
		/*const auto PieMin = 0.125f;
		const auto PieMax = 1.0f - 0.125f;

		return Actions::Collection::MakeSequence(
			Actions::Collection::ChangeCirclePie(circle, PieMin, PieMax, 1.0f, Easing::CubicInOut),
			Actions::Collection::ChangeCirclePie(circle, PieMax, PieMin, 1.0f, Easing::CubicInOut)
		);*/
	}));
}

// tabs manager

void TabsManager::addContent(int type, std::shared_ptr<Item> node)
{
	mContents.insert({ type, node });
	node->onJoin();
}

void TabsManager::addButton(int type, std::shared_ptr<Item> node)
{
	mButtons.insert({ type, node });
	node->onJoin();
}

void TabsManager::show(int type)
{
	if (mCurrentPage.has_value())
	{
		mContents.at(mCurrentPage.value())->onLeave();
		mButtons.at(mCurrentPage.value())->onLeave();
	}

	mContents.at(type)->onEnter();
	mButtons.at(type)->onEnter();
	mCurrentPage = type;
}

// no internet content

NoInternetContent::NoInternetContent()
{
	setStretch(1.0f);

	mLabel = std::make_shared<Helpers::Label>();
	mLabel->setText(LOCALIZE("SOCIAL_NO_INTERNET"));
	//mLabel->setFontSize(24.0f);
	mLabel->setAnchor(0.5f);
	mLabel->setPivot({ 0.5f, 0.0f });
	mLabel->setY(8.0f);
	mLabel->setMultiline(true);
	mLabel->setMultilineAlign(Graphics::TextMesh::Align::Center);
	mLabel->setStretch({ 1.0f, 0.0f });
	mLabel->setMargin({ 48.0f, 0.0f });
	mLabel->setAlpha(0.0f);
	attach(mLabel);

	mIcon = std::make_shared<Scene::Adaptive<Scene::Sprite>>();
	mIcon->setTexture(TEXTURE("textures/no_internet.png"));
	mIcon->setAnchor(0.5f);
	mIcon->setPivot({ 0.5f, 1.0f });
	mIcon->setY(-8.0f);
	mIcon->setAdaptSize(64.0f);
	mIcon->setAlpha(0.0f);
	attach(mIcon);
}

void NoInternetContent::runShowAction()
{
	runAction(Actions::Collection::Delayed(0.25f, Actions::Collection::MakeParallel(
		Actions::Collection::Show(mIcon, 0.25f, Easing::CubicOut),
		Actions::Collection::Show(mLabel, 0.25f, Easing::CubicOut)
	)));
}

// achievement notify

AchievementNotify::AchievementNotify(const Achievements::Item& item)
{
	setSize({ 256.0f, 64.0f });
	setRounding(1.0f);
	setColor(Pallete::WindowItem);

	mTadaHolder = std::make_shared<Scene::Node>();
	mTadaHolder->setAnchor({ 0.0f, 0.5f });
	mTadaHolder->setPivot(0.5f);
	mTadaHolder->setPosition({ 38.0f, 0.0f });
	mTadaHolder->setScale(0.0f);
	attach(mTadaHolder);

	auto tada = std::make_shared<Scene::Adaptive<Scene::Sprite>>();
	tada->setTexture(TEXTURE("textures/tada.png"));
	tada->setAdaptSize(32.0f);
	tada->setAnchor(0.5f);
	tada->setPivot(0.5f);
	mTadaHolder->attach(tada);

	mTadaEmitter = std::make_shared<Scene::RectangleEmitter>();
	mTadaEmitter->setHolder(ParticlesHolder);
	mTadaEmitter->setRunning(false);
	mTadaEmitter->setBeginSize({ 8.0f, 8.0f });
	mTadaEmitter->setStretch({ 1.0f, 0.0f });
	mTadaEmitter->setPivot({ 0.5f, 0.5f });
	mTadaEmitter->setAnchor({ 0.5f, 1.0f });
	mTadaEmitter->setDistance(48.0f);
	mTadaEmitter->setMinDuration(0.25f);
	mTadaEmitter->setMaxDuration(0.75f);
	tada->attach(mTadaEmitter);

	auto title = std::make_shared<Label>();
	title->setColor(Pallete::YellowLabel);
	title->setAnchor({ 0.0f, 0.5f });
	title->setPivot({ 0.0f, 1.0f });
	title->setPosition({ 64.0f, -4.0f });
	title->setText(LOCALIZE("ACHIEVEMENT_" + item.name));
	attach(title);

	auto progress = ACHIEVEMENTS->getProgress(item.name);
	auto required = item.required;

	auto progressbar = std::make_shared<Shared::SceneHelpers::Progressbar>();
	progressbar->setSize({ 148.0f, 4.0f });
	progressbar->setAnchor({ 0.0f, 0.5f });
	progressbar->setPivot({ 0.0f, 1.0f });
	progressbar->setPosition({ 64.0f, 18.0f });
	progressbar->setProgress((float)progress / (float)required);
	attach(progressbar);

	auto progress_label = std::make_shared<Helpers::Label>();
	progress_label->setPosition({ 0.0f, -4.0f });
	progress_label->setAnchor({ 0.0f, 0.0f });
	progress_label->setPivot({ 0.0f, 1.0f });
	progress_label->setFontSize(12.0f);
	progress_label->setText(fmt::format("{}/{}", progress, required));
	progressbar->attach(progress_label);
}

void AchievementNotify::showTada()
{
	runAction(Actions::Collection::MakeSequence(
		Actions::Collection::ChangeScale(mTadaHolder, { 1.0f, 1.0f }, 0.25f, Easing::BackOut),
		Actions::Collection::Execute([this] {
			//mTadaEmitter->emit(16);
		})
	));
}

// rubies indicator

RubiesIndicator::RubiesIndicator()
{
	setTexture(TEXTURE("textures/ruby.png"));
	setPivot({ 0.0f, 0.5f });
	setPosition({ 16.0f, 24.0f });
	setSize(24.0f);
	
	mLabel = std::make_shared<Label>();
	mLabel->setText(std::to_string(PROFILE->getRubies()));
	mLabel->setAnchor({ 1.0f, 0.5f });
	mLabel->setPivot({ 0.0f, 0.5f });
	mLabel->setPosition({ 8.0f, 0.0f });
	attach(mLabel);
}

void RubiesIndicator::onEvent(const Profile::RubiesChangedEvent& e)
{
	if (!mInstantRefresh)
		return;

	refresh();
}

void RubiesIndicator::refresh()
{
	mLabel->setText(std::to_string(PROFILE->getRubies()));
}

void RubiesIndicator::collectRubyAnim(std::shared_ptr<Scene::Node> ruby)
{
	auto pos = unproject(ruby->project({ 0.0f, 0.0f }));
	ruby->setAnchor({ 0.0f, 0.0f });
	ruby->setPivot({ 0.0f, 0.0f });
	ruby->setPosition(pos);
	ruby->getParent()->detach(ruby);
	attach(ruby);

	const float MoveDuration = 0.75f;

	auto rubies_count = PROFILE->getRubies();

	runAction(Actions::Collection::MakeSequence(
		Actions::Collection::MakeParallel(
			Actions::Collection::ChangePosition(ruby, { 0.0f, 0.0f }, MoveDuration, Easing::QuarticInOut),
			Actions::Collection::ChangeSize(ruby, getAbsoluteSize(), MoveDuration, Easing::QuarticInOut)
		),
		Actions::Collection::Kill(ruby),
		Actions::Collection::Execute([this, rubies_count] {
			mLabel->setText(std::to_string(rubies_count));
		}),
		Actions::Collection::Shake(mLabel, 2.0f, 0.2f)
	));
}

void RubiesIndicator::makeHidden()
{
	setAlpha(0.0f);
	mLabel->setAlpha(0.0f);
}

void RubiesIndicator::show()
{
	runAction(Actions::Collection::Show(shared_from_this(), 0.25f));
	runAction(Actions::Collection::Show(mLabel, 0.25f));
}

void RubiesIndicator::hide()
{
	runAction(Actions::Collection::Hide(shared_from_this(), 0.25f));
	runAction(Actions::Collection::Hide(mLabel, 0.25f));
}