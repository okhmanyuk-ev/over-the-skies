#include "hud.h"
#include "profile.h"

using namespace hcg001;

Hud::Hud()
{
	// ruby score sprite

	mRubyScore.sprite = std::make_shared<Scene::Sprite>();
	mRubyScore.sprite->setTexture(TEXTURE("textures/ruby.png"));
	mRubyScore.sprite->setPivot({ 0.0f, 0.5f });
	mRubyScore.sprite->setPosition({ 16.0f, 24.0f });
	mRubyScore.sprite->setSize(24.0f);
	attach(mRubyScore.sprite);

	// ruby score label

	mRubyScore.label = std::make_shared<Scene::Label>();
	mRubyScore.label->setFont(FONT("default"));
	mRubyScore.label->setText(std::to_string(PROFILE->getRubies()));
	mRubyScore.label->setAnchor({ 1.0f, 0.5f });
	mRubyScore.label->setPivot({ 0.0f, 0.5f });
	mRubyScore.label->setPosition({ 8.0f, 0.0f });
	mRubyScore.sprite->attach(mRubyScore.label);
}

void Hud::collectRubyAnim(std::shared_ptr<Scene::Node> ruby)
{
	auto pos = unproject(ruby->project({ 0.0f, 0.0f }));
	ruby->setAnchor({ 0.0f, 0.0f });
	ruby->setPivot({ 0.0f, 0.0f });
	ruby->setPosition(pos);
	ruby->getParent()->detach(ruby);
	attach(ruby);

	auto dest_pos = unproject(mRubyScore.sprite->project({ 0.0f, 0.0f }));

	const float MoveDuration = 0.75f;

	Actions::Run(Actions::Factory::MakeSequence(
		Actions::Factory::MakeParallel(
			Actions::Factory::ChangePosition(ruby, dest_pos, MoveDuration, Easing::QuarticInOut),
			Actions::Factory::ChangeSize(ruby, mRubyScore.sprite->getAbsoluteSize(), MoveDuration, Easing::QuarticInOut)
		),
		Actions::Factory::Kill(ruby),
		Actions::Factory::Execute([this] {
			mRubyScore.label->setText(std::to_string(PROFILE->getRubies()));
		}),
		Actions::Factory::Shake(mRubyScore.label, 2.0f, 0.2f)
	));
}