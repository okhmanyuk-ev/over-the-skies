#pragma once

#include <shared/all.h>
#include "hud.h"
#include "client.h"
#include "profile.h"

namespace hcg001::Helpers
{
	const auto BaseWindowColor = glm::rgbColor(glm::vec3(210.0f, 0.5f, 1.0f));
	const auto ButtonColor = glm::rgbColor(glm::vec3(209.0f, 0.5f, 0.38f));

	inline std::shared_ptr<Hud> gHud = nullptr;

	struct PrintEvent
	{
		std::string text;
	};

	struct HighscoresEvent
	{
		std::vector<int> uids;
	};

	struct ProfileReceived // TODO: move to client.h
	{
		int uid;
	};

	class Label : public Scene::Label
	{
	public:
		Label();
	};

	class Button : public Shared::SceneHelpers::BouncingButtonBehavior<Shared::SceneHelpers::Button<Scene::Rectangle>>
	{
	public:
		Button();

	public:
		void refresh() override {}

	public:
		auto getLabel() const { return mLabel; }

		auto isAdaptiveFontSize() const { return mAdaptiveFontSize; }
		void setAdaptiveFontSize(bool value) { mAdaptiveFontSize = value; }

	private:
		std::shared_ptr<Label> mLabel;
		bool mAdaptiveFontSize = true;
	};	
	
	class RectangleButton : public Shared::SceneHelpers::BouncingButtonBehavior<Shared::SceneHelpers::RectangleButton>
	{
	public:
		RectangleButton();
	};

	class TextInputField : public Shared::SceneHelpers::BouncingButtonBehavior<Scene::Clickable<Scene::ClippableStencil<Scene::Rectangle>>>
	{
	public:
		TextInputField();

	public:
		auto getLabel() const { return mLabel; }

	private:
		std::shared_ptr<Label> mLabel;
	};

	template <class T> class ProfileListenable : public T, public Common::Event::Listenable<ProfileReceived>
	{
	public:
		using ProfileCallback = std::function<void(Channel::ProfilePtr)>;

	private:
		void onEvent(const ProfileReceived& e) override
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

	template <class T> class GuildInfoListenable : public T, public Common::Event::Listenable<Channel::GuildInfoReceivedEvent>
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

	float SmoothValueSetup(float src, float dst);
}