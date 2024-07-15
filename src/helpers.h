#pragma once

#include <shared/all.h>
#include "profile.h"
#include "achievements.h"
#include "sky.h"

namespace hcg001
{
	class MainMenu;
}

namespace hcg001::Helpers
{
	namespace Pallete
	{
		const float Hue = 200.0f;

		const auto WindowHead = glm::rgbColor(glm::vec3(Hue, 0.75f + 0.125f, 0.25f));
		const auto WindowBody = glm::rgbColor(glm::vec3(Hue, 0.75f + 0.125f, 0.125f));
		const auto WindowTitle = Graphics::Color::ToNormalized(255, 255, 255);
		const auto WindowItem = glm::rgbColor(glm::vec3(Hue, 0.4f, 0.25f));
		const auto YellowLabel = glm::rgbColor(glm::vec3(60.0f, 0.25f, 1.0f));
		const auto ButtonColor = glm::rgbColor(glm::vec3(Hue, 0.75f + 0.125f, 0.25f + 0.125f));
		const auto ButtonColorRed = glm::rgbColor(glm::vec3(20.0f, 0.75f + 0.125f, 0.25f + 0.125f));
		const auto InputField = glm::rgbColor(glm::vec3(Hue, 0.75f + 0.125f, 0.125f / 2.0f));
	}

	inline std::shared_ptr<Sky> gSky = nullptr;
	inline std::shared_ptr<MainMenu> gMainMenu = nullptr;

	class Label : public Scene::Label
	{
	public:
		Label();
	};

	class Button : public Shared::SceneHelpers::BouncingButtonBehavior<Shared::SceneHelpers::RectangleButton>
	{
	public:
		Button();

	public:
		void setButtonColor(const glm::vec3& color);
	};

	using SpriteButton = Shared::SceneHelpers::BouncingButtonBehavior<Shared::SceneHelpers::SpriteButton>;
	using AdaptiveSpriteButton = Shared::SceneHelpers::BouncingButtonBehavior<Scene::Adaptive<Shared::SceneHelpers::SpriteButton>>;

	class AchievementNotify : public Scene::ClippableStencil<Scene::Rectangle>
	{
	public:
		static inline std::shared_ptr<Scene::Node> ParticlesHolder = nullptr;

	public:
		AchievementNotify(const Achievements::Item& item);

	public:
		void showTada();

	private:
		std::shared_ptr<Scene::Adaptive<Scene::Sprite>> mTada;
	};

	class RubiesIndicator : public Scene::Sprite,
		public Common::Event::Listenable<Profile::RubiesChangedEvent>,
		public std::enable_shared_from_this<RubiesIndicator>
	{
	public:
		RubiesIndicator();

	private:
		void onEvent(const Profile::RubiesChangedEvent& e) override;

	public:
		void refresh();
		void collectRubyAnim(std::shared_ptr<Scene::Node> ruby, float delay = 0.0f);
		void makeHidden();
		void show();
		void hide();

	private:
		std::shared_ptr<Label> mLabel;

	public:
		void setInstantRefresh(bool value) { mInstantRefresh = value; }

	private:
		bool mInstantRefresh = true;
	};
}
