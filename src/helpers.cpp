#include "helpers.h"

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

// achievement notify

AchievementNotify::AchievementNotify(const Achievements::Item& item)
{
	setSize({ 256.0f, 64.0f });
	setRounding(1.0f);
	setColor(Pallete::WindowItem);

	mTada = std::make_shared<Scene::Adaptive<Scene::Sprite>>();
	mTada->setTexture(TEXTURE("textures/tada.png"));
	mTada->setAdaptSize(32.0f);
	mTada->setAnchor({ 0.0f, 0.5f });
	mTada->setPivot(0.5f);
	mTada->setPosition({ 38.0f, 0.0f });
	mTada->setScale(0.0f);
	attach(mTada);

	/*mTadaEmitter = std::make_shared<Scene::RectangleEmitter>(); // TODO: wtf
	mTadaEmitter->setHolder(ParticlesHolder);
	mTadaEmitter->setRunning(false);
	mTadaEmitter->setBeginSize({ 8.0f, 8.0f });
	mTadaEmitter->setStretch({ 1.0f, 0.0f });
	mTadaEmitter->setPivot({ 0.5f, 0.5f });
	mTadaEmitter->setAnchor({ 0.5f, 1.0f });
	mTadaEmitter->setDistance(48.0f);
	mTadaEmitter->setMinDuration(0.25f);
	mTadaEmitter->setMaxDuration(0.75f);
	mTada->attach(mTadaEmitter);*/

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
	progress_label->setText(fmt::format(L"{}/{}", progress, required));
	progressbar->attach(progress_label);
}

void AchievementNotify::showTada()
{
	runAction(Actions::Collection::MakeSequence(
		Actions::Collection::ChangeScale(mTada, { 1.0f, 1.0f }, 0.25f, Easing::BackOut),
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
	mLabel->setFontSize(22.0f);
	mLabel->setText(std::to_wstring(PROFILE->getRubies()));
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
	mLabel->setText(std::to_wstring(PROFILE->getRubies()));
}

void RubiesIndicator::collectRubyAnim(std::shared_ptr<Scene::Node> ruby, float delay)
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
		Actions::Collection::Wait(delay),
		Actions::Collection::MakeParallel(
			Actions::Collection::ChangePosition(ruby, { 0.0f, 0.0f }, MoveDuration, Easing::QuarticInOut),
			Actions::Collection::ChangeSize(ruby, getAbsoluteSize(), MoveDuration, Easing::QuarticInOut)
		),
		Actions::Collection::Kill(ruby),
		Actions::Collection::Execute([this, rubies_count] {
			mLabel->setText(std::to_wstring(rubies_count));
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
