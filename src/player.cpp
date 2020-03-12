#include "player.h"

using namespace hcg001;

Player::Player(Skin skin) : mSkin(skin)
{
	setSize(18.0f);
	setPivot({ 0.5f, 0.5f });

	mSprite = std::make_shared<Scene::Sprite>(); // we should use sprite as child and rotate only this child
	mSprite->setTexture(TEXTURE(SkinPath.at(skin)));
	mSprite->setStretch(1.0f);
	mSprite->setAnchor({ 0.5f, 0.5f });
	mSprite->setPivot({ 0.5f, 0.5f });
	attach(mSprite);
}

void Player::update()
{
	Scene::Actionable<Scene::Node>::update();
	mSprite->setColor(getColor());
}

BallPlayer::BallPlayer(std::weak_ptr<Scene::Node> trailHolder) : Player(Skin::Ball)
{
	auto trail = std::make_shared<Scene::Trail>(trailHolder);
	trail->setAnchor({ 0.5f, 0.5f });
	trail->setPivot({ 0.5f, 0.5f });
	trail->setStretch({ 0.9f, 0.9f });
	trail->setLifetime(0.2f);
	trail->setNarrowing(true);
	attach(trail);
}

SnowflakePlayer::SnowflakePlayer(std::weak_ptr<Scene::Node> trailHolder) : Player(Skin::Snowflake)
{
	auto trail = std::make_shared<Scene::Trail>(trailHolder);
	trail->setColor(Graphics::Color::ToNormalized(193, 255, 255, 127));
	trail->setAnchor({ 0.5f, 0.5f });
	trail->setPivot({ 0.5f, 0.5f });
	trail->setStretch({ 0.9f, 0.9f });
	trail->setLifetime(0.2f);
	trail->setNarrowing(true);
	attach(trail);
}

FlamePlayer::FlamePlayer(std::weak_ptr<Scene::Node> trailHolder) : Player(Skin::Flame)
{
	auto emitter = std::make_shared<Shared::SceneHelpers::SpriteEmitter>(trailHolder);
	emitter->setPivot({ 0.5f, 0.5f });
	emitter->setAnchor({ 0.5f, 0.5f });
	emitter->setTexture(TEXTURE("textures/point_light_texture.png"));
	emitter->setDelay(1.0f / 60.0f);
	emitter->setBeginSize({ 32.0f, 32.0f });
	emitter->setDistance(4.0f);
	emitter->setSampler(Renderer::Sampler::Linear);
	//emitter->setBlendMode(Renderer::BlendMode(Renderer::Blend::SrcAlpha, Renderer::Blend::One));
//	emitter->setBlendMode(Renderer::BlendMode(Renderer::Blend::SrcAlpha, Renderer::Blend::InvSrcAlpha,
//		Renderer::Blend::One, Renderer::Blend::InvSrcAlpha);

	emitter->setBlendMode(Renderer::BlendMode(Renderer::Blend::SrcAlpha, Renderer::Blend::InvSrcAlpha,
		Renderer::Blend::One, Renderer::Blend::InvSrcAlpha));

	attach(emitter);
}
