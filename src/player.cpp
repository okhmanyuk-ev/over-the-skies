#include "player.h"

using namespace hcg001;

Player::Player(Skin skin) : mSkin(skin)
{
	setSize(18.0f);
	setPivot({ 0.5f, 0.5f });
}

void Player::makeSprite()
{
	mSprite = std::make_shared<Scene::Sprite>(); // we should use sprite as child and rotate only this child
	mSprite->setTexture(TEXTURE(SkinPath.at(mSkin)));
	mSprite->setStretch(1.0f);
	mSprite->setAnchor({ 0.5f, 0.5f });
	mSprite->setPivot({ 0.5f, 0.5f });
	mSprite->setSampler(Renderer::Sampler::Linear);
	attach(mSprite);
}

void Player::update()
{
	assert(mSprite != nullptr);
	Scene::Node::update();
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

	makeSprite();
}

SnowflakePlayer::SnowflakePlayer(std::weak_ptr<Scene::Node> trailHolder) : Player(Skin::Snowflake)
{
	auto trail = std::make_shared<Scene::Trail>(trailHolder);
	trail->setColor(Graphics::Color::ToNormalized(193, 255, 255));
	trail->setAnchor({ 0.5f, 0.5f });
	trail->setPivot({ 0.5f, 0.5f });
	trail->setStretch({ 0.9f, 0.9f });
	trail->setLifetime(0.2f);
	trail->setNarrowing(true);
	attach(trail);

	makeSprite();
}

BasketballPlayer::BasketballPlayer(std::weak_ptr<Scene::Node> trailHolder) : Player(Skin::Basketball)
{
	/*auto emitter = std::make_shared<Shared::SceneHelpers::SpriteEmitter>(trailHolder);
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

	attach(emitter);*/

	auto trail = std::make_shared<Scene::Trail>(trailHolder);
	trail->setAnchor({ 0.5f, 0.5f });
	trail->setPivot({ 0.5f, 0.5f });
	trail->setStretch({ 0.9f, 0.9f });
	trail->setLifetime(0.2f);
	trail->setColor(Graphics::Color::ToNormalized(244, 211, 173));
	trail->setNarrowing(true);
	attach(trail);

	makeSprite();
}

FootballPlayer::FootballPlayer(std::weak_ptr<Scene::Node> trailHolder) : Player(Skin::Football)
{
	auto trail = std::make_shared<Scene::Trail>(trailHolder);
	trail->setAnchor({ 0.5f, 0.5f });
	trail->setPivot({ 0.5f, 0.5f });
	trail->setStretch({ 0.9f, 0.9f });
	trail->setLifetime(0.2f);
	trail->setNarrowing(true);
	attach(trail);

	makeSprite();
}

VynilPlayer::VynilPlayer(std::weak_ptr<Scene::Node> trailHolder) : Player(Skin::Vynil)
{
	auto trail = std::make_shared<Scene::Trail>(trailHolder);
	trail->setAnchor({ 0.5f, 0.5f });
	trail->setPivot({ 0.5f, 0.5f });
	trail->setStretch({ 0.9f, 0.9f });
	trail->setLifetime(0.2f);
	trail->setNarrowing(true);
	attach(trail);

	makeSprite();
}

PayablePlayer::PayablePlayer(std::weak_ptr<Scene::Node> trailHolder) : Player(Skin::Payable)
{
	/*auto trail = std::make_shared<Scene::Trail>(trailHolder);
	trail->setAnchor({ 0.5f, 0.5f });
	trail->setPivot({ 0.5f, 0.5f });
	trail->setStretch({ 0.9f, 0.9f });
	trail->setLifetime(0.2f);
	trail->setNarrowing(true);
	attach(trail);*/

	auto circle_texture = std::make_shared<Renderer::RenderTarget>(24, 24);

	auto model = glm::mat4(1.0f);
	model = glm::scale(model, { (float)circle_texture->getWidth(), (float)circle_texture->getHeight(), 1.0f });

	GRAPHICS->begin();
	GRAPHICS->pushRenderTarget(circle_texture);
	GRAPHICS->pushOrthoMatrix(circle_texture);
	GRAPHICS->pushBlendMode(Renderer::BlendStates::AlphaBlend);
	GRAPHICS->pushViewport(circle_texture);
	GRAPHICS->clear();
	GRAPHICS->drawCircle(model, { Graphics::Color::White, 1.0f }, { Graphics::Color::White, 0.0f });
	GRAPHICS->pop(4);
	GRAPHICS->end();

	auto emitter = std::make_shared<Scene::SpriteEmitter>();
	emitter->setHolder(trailHolder);
	emitter->setStretch(0.0f);
	emitter->setPivot(0.5f);
	emitter->setAnchor(0.5f);
	emitter->setTexture(circle_texture);			
	emitter->setBlendMode(Renderer::BlendStates::Additive);
	emitter->setSampler(Renderer::Sampler::Linear);
	emitter->setDelay(1.0f / 120.0f);
	emitter->setDirection({ 0.0f, 0.0f });
	emitter->setBeginColor({ 1.0f, 0.25f, 1.0f, 1.0f });
	emitter->setEndColor({ 0.25f, 0.25f, 1.0f, 1.0f });
	//emitter->setBeginScale({ 1.0f, 1.0f });
	//emitter->setEndScale({ 1.0f, 1.0f });
	attach(emitter);

	makeSprite();
}