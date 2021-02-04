#pragma once

#include <shared/all.h>
#include "profile.h"

#define CLIENT ENGINE->getSystem<hcg001::Client>()

namespace hcg001
{
	struct GlobalChatMessageEvent
	{
		int uid;
		std::string text;
	};

	class Channel : public Shared::NetworkingUDP::SimpleChannel
	{
	public:
		using ProfilePtr = std::shared_ptr<const Profile>;
		using ProfilesMap = std::map<int/*id*/, ProfilePtr>;

	public:
		Channel();

	public:
		void auth();
		void commit();
		void requestHighscores();
		void requestProfile(int uid);
		void clearProfiles();
		void sendChatMessage(const std::string& text);

	private:
		void readFileMessage(Common::BitBuffer& buf);

	public:
		const auto& getProfiles() const { return mProfiles; }

	private:
		int mUID = 0;

	private:
		struct FileDef
		{
			size_t progress = 0;
			Common::BitBuffer buf;
		};
		std::map<std::string, FileDef> mFiles;

		ProfilesMap mProfiles;

	private:
		std::string mPrevProfileDump;
	};

	class Client : public Shared::NetworkingUDP::Client
	{
	public:
		Client();

	protected:
		std::shared_ptr<Shared::NetworkingUDP::Channel> createChannel() override;

	public:
		void frame() override;

	public:
		void commit();
		void requestHighscores();
		void requestProfile(int uid);
		void requireProfile(int uid);
		void sendChatMessage(const std::string& text);

	private:
		auto getMyChannel() const { return std::dynamic_pointer_cast<Channel>(getChannel()); }

	public:
		const Channel::ProfilesMap&  getProfiles() const;
		bool hasProfile(int uid);
		Channel::ProfilePtr getProfile(int uid);
		void clearProfiles();
	};
}
