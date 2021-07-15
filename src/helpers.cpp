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