#pragma once

#include <shared/all.h>
#include "profile.h"

#define CLIENT ENGINE->getSystem<hcg001::Client>()

namespace hcg001
{
	class Channel : public Shared::NetworkingUDP::SimpleChannel
	{
	public:
		class ChatMessage;

	public:
		using ProfilePtr = std::shared_ptr<const Profile>;
		using ProfilesMap = std::map<int/*id*/, ProfilePtr>;
		using GlobalChatMessages = std::map<int, std::shared_ptr<ChatMessage>>;

	public:
		struct GlobalChatMessageEvent
		{
			int msgid; // TODO: rename to index
		};

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
		const auto& getGlobalChatMessages() const { return mGlobalChatMessages; }

	public:
		auto getUID() const { return mUID; }

	private:
		int mUID = 0;

	private:
		struct FileDef
		{
			size_t progress = 0;
			Common::BitBuffer buf;
		};
		std::map<std::string, FileDef> mFiles;

	private:
		ProfilesMap mProfiles;
		GlobalChatMessages mGlobalChatMessages;

	private:
		std::string mPrevProfileDump;
	};

	class Channel::ChatMessage
	{
	public:
		ChatMessage(int uid, const std::string& text);

	public:
		auto getUID() const { return mUID; }
		const auto& getText() const { return mText; }

	private:
		int mUID;
		std::string mText;
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
		
	public:
		const Channel::GlobalChatMessages& getGlobalChatMessages() const;

	private:
		auto getMyChannel() const { return std::dynamic_pointer_cast<Channel>(getChannel()); }

	public:
		const Channel::ProfilesMap& getProfiles() const;
		bool hasProfile(int uid);
		Channel::ProfilePtr getProfile(int uid);
		void clearProfiles();

	public:
		int getUID() const;
	};
}
