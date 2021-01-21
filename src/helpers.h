#pragma once

#include <shared/all.h>
#include "hud.h"
#include "client.h"
#include "profile.h"

namespace hcg001::Helpers
{
	const float ButtonRounding = 0.33f;

	const auto BaseWindowColor = glm::rgbColor(glm::vec3(210.0f, 0.5f, 1.0f));

	inline std::shared_ptr<Hud> gHud = nullptr;

	struct PrintEvent
	{
		std::string text;
	};

	struct HighscoresEvent
	{
		std::vector<int> uids;
	};

	struct ProfileReceived
	{
		int uid;
	};

	class Label : public Scene::Label
	{
	public:
		Label();
	};

	class TextInputField : public Scene::Clickable<Scene::Node>
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
}