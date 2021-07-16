#pragma once

#include <shared/all.h>
#include "hud.h"
#include "client.h"
#include "profile.h"
#include "achievements.h"

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

	inline std::shared_ptr<Hud> gHud = nullptr;

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

	public:
		auto isAdaptiveFontSize() const { return mAdaptiveFontSize; }
		void setAdaptiveFontSize(bool value) { mAdaptiveFontSize = value; }

	private:
		bool mAdaptiveFontSize = true;
	};	
	
	class TextInputField : public Shared::SceneHelpers::BouncingButtonBehavior<Scene::Clickable<Scene::ClippableStencil<Scene::Rectangle>>>
	{
	public:
		TextInputField(const utf8_string& input_window_title);

	public:
		auto getLabel() const { return mLabel; }

	private:
		std::shared_ptr<Label> mLabel;
	};

	template <class T> class ProfileListenable : public T, public Common::Event::Listenable<NetEvents::ProfileReceived>
	{
	public:
		using ProfileCallback = std::function<void(Channel::ProfilePtr)>;

	private:
		void onEvent(const NetEvents::ProfileReceived& e) override
		{
			if (e.uid != mProfileUID)
				return;

			mFirstCalled = true;

			if (mProfileCallback)
				mProfileCallback(CLIENT->getProfile(mProfileUID));
		}

	protected:
		void update() override
		{
			T::update();
			
			if (mFirstCalled)
				return;
			
			mFirstCalled = true;

			if (!CLIENT->hasProfile(mProfileUID))
				return;

			if (mProfileCallback)
				mProfileCallback(CLIENT->getProfile(mProfileUID));
		}

	public:
		auto getProfileUID() const { return mProfileUID; }
		void setProfileUID(int value) { mProfileUID = value; }
		
		auto getProfileCallback() const { return mProfileCallback; }
		void setProfileCallback(ProfileCallback value) { mProfileCallback = value; }

	private:
		int mProfileUID = 0;
		ProfileCallback mProfileCallback;
		bool mFirstCalled = false;
	};

	template <class T> class GuildInfoListenable : public T, 
		public Common::Event::Listenable<Channel::GuildInfoReceivedEvent>
	{
	public:
		using GuildCallback = std::function<void(Channel::GuildPtr)>;

	private:
		void onEvent(const Channel::GuildInfoReceivedEvent& e) override
		{
			if (e.id != mGuildID)
				return;

			mFirstCalled = true;

			if (mGuildCallback)
				mGuildCallback(CLIENT->getGuild(mGuildID));
		}

	protected:
		void update() override
		{
			T::update();
			
			if (mFirstCalled)
				return;
			
			mFirstCalled = true;

			if (!CLIENT->hasGuild(mGuildID))
				return;

			if (mGuildCallback)
				mGuildCallback(CLIENT->getGuild(mGuildID));
		}

	public:
		auto getGuildID() const { return mGuildID; }
		void setGuildID(int value) { mGuildID = value; }
		
		auto getGuildCallback() const { return mGuildCallback; }
		void setGuildCallback(GuildCallback value) { mGuildCallback = value; }

	private:
		int mGuildID = 0;
		GuildCallback mGuildCallback;
		bool mFirstCalled = false;
	};

	float SmoothValueSetup(float src, float dst, Clock::Duration dTime);

	class WaitingIndicator : public Scene::Node
	{
	public:
		WaitingIndicator();
	};

	class TabsManager
	{
	public:
		class Item;

	public:
		void addContent(int type, std::shared_ptr<Item> node);
		void addButton(int type, std::shared_ptr<Item> node);
		void show(int type);

	public:
		const auto& getContents() const { return mContents; }

	private:
		std::map<int, std::shared_ptr<Item>> mContents;
		std::map<int, std::shared_ptr<Item>> mButtons;
		std::optional<int> mCurrentPage;
	};

	class TabsManager::Item
	{
	public:
		virtual void onJoin() = 0;
		virtual void onEnter() = 0;
		virtual void onLeave() = 0;
	};

	template <class T> class MappedTabsManager : public TabsManager
	{
		static_assert(std::is_enum<T>::value, "T must be enum");
	public:
		void addContent(T type, std::shared_ptr<Item> node) { TabsManager::addContent((int)type, node); }
		void addButton(T type, std::shared_ptr<Item> node) { TabsManager::addButton((int)type, node); }
		void show(T type) { TabsManager::show((int)type); }
	};

	class NoInternetContent : public Scene::Node
	{
	public:
		NoInternetContent();

	public:
		void runShowAction();

	private:
		std::shared_ptr<Helpers::Label> mLabel;
		std::shared_ptr<Scene::Adaptive<Scene::Sprite>> mIcon;
	};

	class AchievementNotify : public Scene::ClippableStencil<Scene::Rectangle>
	{
	public:
		static inline std::shared_ptr<Scene::Node> ParticlesHolder = nullptr;

	public:
		AchievementNotify(const Achievements::Item& item);

	public:
		void showTada();

	private:
		std::shared_ptr<Scene::Node> mTadaHolder;
		std::shared_ptr<Shared::SceneHelpers::RectangleEmitter> mTadaEmitter;
	};
}