#include "global_chat_window.h"
#include "helpers.h"
#include "input_window.h"

using namespace hcg001;

bool GlobalChatWidget::isHistoryEmpty() const
{
	const auto& history = CLIENT->getGlobalChatMessages();
	return history.empty();
}

int GlobalChatWidget::getFirstHistoryIndex() const
{
	const auto& history = CLIENT->getGlobalChatMessages();
	return history.begin()->first;
}

int GlobalChatWidget::getLastHistoryIndex() const
{
	const auto& history = CLIENT->getGlobalChatMessages();
	return history.rbegin()->first;
}

bool GlobalChatWidget::hasIndexInHistory(int index) const
{
	const auto& history = CLIENT->getGlobalChatMessages();
	return history.count(index);
}

int GlobalChatWidget::getProfileIdFromMessage(int index) const
{
	const auto& history = CLIENT->getGlobalChatMessages();
	return history.at(index)->getUID();
}

std::shared_ptr<Scene::Node> GlobalChatWidget::createMessageNode(int index) const
{
	auto item = std::make_shared<Scene::Cullable<Scene::Node>>();
	item->setStretch({ 1.0f, 0.0f });
	item->setCullTarget(getScrollbox());
	item->setVisible(false);
	getScrollbox()->getContent()->attach(item);

	auto holder = std::make_shared<Scene::Node>();
	holder->setStretch({ 0.8f, 1.0f });
	holder->setMargin({ 0.0f, 8.0f });
	holder->setAnchor({ 0.0f, 0.5f });
	holder->setPivot({ 0.0f, 0.5f });
	holder->setX(8.0f);
	item->attach(holder);

	//auto rect = std::make_shared<Scene::ClippableStencil<Scene::Rectangle>>(); // TODO: fix bug with stencil and scrollbar
	//auto rect = std::make_shared<Scene::ClippableScissor<Scene::Rectangle>>(); // TODO: fix bug with this shit

	auto rect = std::make_shared<Scene::Rectangle>();
	rect->setStretch(1.0f);
	rect->setAnchor(0.5f);
	rect->setPivot(0.5f);
	rect->setRounding(8.0f);
	rect->setAbsoluteRounding(true);
	rect->setColor(Helpers::Pallete::WindowItem);
	holder->attach(rect);

	const auto& history = CLIENT->getGlobalChatMessages();
	const auto& msg = history.at(index);

	assert(CLIENT->hasProfile(msg->getUID()));
	auto profile = CLIENT->getProfile(msg->getUID());

	auto label = std::make_shared<Helpers::Label>();
	label->setFontSize(15.0f);
	label->setAnchor(0.5f);
	label->setPivot(0.5f);
	label->setStretch({ 1.0f, 0.0f });
	label->setMargin({ 24.0f, 0.0f });
	label->setMultiline(true);
	label->setAutoRefreshing(false);
	label->setText(profile->getNickName() + ": " + msg->getText());
	rect->attach(label);

	item->updateAbsoluteSize();
	holder->updateAbsoluteSize();
	rect->updateAbsoluteSize();
	label->updateAbsoluteSize();
	label->refresh();

	item->setHeight(label->getAbsoluteHeight() + 24.0f);

	auto text_width = label->getFont()->getStringWidth(label->getText(), label->getFontSize());
	auto abs_width = label->getAbsoluteWidth();

	if (text_width < abs_width)
		holder->setHorizontalSize(holder->getHorizontalSize() - (abs_width - text_width));

	rect->setScale(0.0f);
	rect->runAction(Actions::Collection::ChangeScale(rect, { 1.0f, 1.0f }, 0.25f, Easing::CubicInOut));

	return item;
}

GlobalChatWindow::GlobalChatWindow()
{
	getBackground()->setSize({ 314.0f, 512.0f });
	getTitle()->setText(LOCALIZE("GLOBAL_CHAT_WINDOW_TITLE"));

	auto no_internet_content = std::make_shared<Helpers::NoInternetContent>();
	getBackground()->attach(no_internet_content);

	auto chat_button = std::make_shared<Helpers::Button>();
	chat_button->setClickCallback([] {
		auto window = std::make_shared<InputWindow>(LOCALIZE("INPUT_CHAT_MESSAGE"), "", [](auto str) {
			CLIENT->sendChatMessage(str.cpp_str());
		});
		SCENE_MANAGER->pushWindow(window);
	});
	chat_button->setAnchor(1.0f);
	chat_button->setPivot({ 1.0f, 0.5f });
	chat_button->setPosition({ -16.0f, -24.0f });
	chat_button->setSize({ 96.0f, 28.0f });
	chat_button->setEnabled(false);
	getBody()->attach(chat_button);

	auto chat_message_img = std::make_shared<Scene::Adaptive<Scene::Sprite>>();
	chat_message_img->setTexture(TEXTURE("textures/chat_message.png"));
	chat_message_img->setAdaptSize({ 48.0f, 20.0f });
	chat_message_img->setAnchor(0.5f);
	chat_message_img->setPivot(0.5f);
	chat_button->attach(chat_message_img);

	mChatWidget = std::make_shared<GlobalChatWidget>();
	mChatWidget->setStretch(1.0f);
	mChatWidget->setVerticalMargin(48.0f);
	mChatWidget->setEnabled(false);
	getBody()->attach(mChatWidget);

	runAction(Actions::Collection::MakeSequence(
		Actions::Collection::Wait([this] { return getState() != Window::State::Opened; }),
		Actions::Collection::Execute([this, no_internet_content] {
			no_internet_content->runShowAction();
		}),
		Actions::Collection::Wait([] {
			return !CLIENT->isConnected();
		}),
		Actions::Collection::Execute([this, chat_button, no_internet_content] {
			no_internet_content->setEnabled(false);
			mChatWidget->setEnabled(true);
			chat_button->setEnabled(true);
		})
	));

	auto top_gradient = std::make_shared<Scene::Rectangle>();
	top_gradient->setStretch({ 1.0f, 0.0f });
	top_gradient->setHeight(6.0f);
	top_gradient->setAnchor({ 0.5f, 0.0f });
	top_gradient->setPivot({ 0.5f, 0.0f });
	top_gradient->getEdgeColor(Scene::Rectangle::Edge::Top)->setColor({ Helpers::Pallete::WindowBody, 1.0f });
	top_gradient->getEdgeColor(Scene::Rectangle::Edge::Bottom)->setColor({ Helpers::Pallete::WindowBody, 0.0f });
	mChatWidget->attach(top_gradient);

	auto bottom_gradient = std::make_shared<Scene::Rectangle>();
	bottom_gradient->setStretch({ 1.0f, 0.0f });
	bottom_gradient->setHeight(6.0f);
	bottom_gradient->setAnchor({ 0.5f, 1.0f });
	bottom_gradient->setPivot({ 0.5f, 1.0f });
	bottom_gradient->getEdgeColor(Scene::Rectangle::Edge::Top)->setColor({ Helpers::Pallete::WindowBody, 0.0f });
	bottom_gradient->getEdgeColor(Scene::Rectangle::Edge::Bottom)->setColor({ Helpers::Pallete::WindowBody, 1.0f });
	mChatWidget->attach(bottom_gradient);
}

void GlobalChatWindow::onEvent(const Channel::GlobalChatMessageEvent& e)
{
	mChatWidget->message(e.msgid);
}