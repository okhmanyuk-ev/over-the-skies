#pragma once

#include <shared/all.h>
#include "profile.h"
#include "guild.h"

#define CLIENT ENGINE->getSystem<hcg001::Client>()

namespace hcg001
{
	class Channel : public Shared::NetworkingWS::SimpleChannel
	{
	public:
		class ChatMessage;

	public:
		using ProfilePtr = std::shared_ptr<const Profile>;
		using ProfilesMap = std::map<int/*id*/, ProfilePtr>;
		using GlobalChatMessages = std::map<int, std::shared_ptr<ChatMessage>>;
		using GuildPtr = std::shared_ptr<const Guild>;
		using GuildsMap = std::map<int/*id*/, GuildPtr>;

	public:
		struct GlobalChatMessageEvent
		{
			int msgid; // TODO: rename to index
		};

		struct CreateGuildEvent
		{
			std::string status;
			std::optional<int> id;
		};

		struct GuildListEvent
		{
			std::vector<int> ids;
		};

		struct GuildInfoReceivedEvent
		{
			int id;
		};

		struct JoinedToGuildEvent
		{
			int id;
		};

		struct ExitedFromGuildEvent { };

	public:
		Channel();

	public:
		void auth();
		void commit();
		void requestHighscores();
		void requestProfile(int uid);
		void clearProfiles();
		void sendChatMessage(const std::string& text);
		void createGuild(const std::string& title);
		void requestGuildList();
		void clearGuilds();
		void requestGuildInfo(int id);
		void exitGuild();
		void joinGuild(int id);

	private:
		void readFileMessage(Common::BitBuffer& buf);

	public:
		const auto& getProfiles() const { return mProfiles; }
		const auto& getGlobalChatMessages() const { return mGlobalChatMessages; }
		const auto& getGuilds() const { return mGuilds; }

	public:
		auto getUID() const { return mUID; }

	private:
		int mUID = 0;

	private:
		struct FileDef
		{
			size_t progress = 0;
			uint32_t crc32;
			Common::BitBuffer buf;
		};
		std::map<std::string, FileDef> mFiles;

	private:
		ProfilesMap mProfiles;
		GlobalChatMessages mGlobalChatMessages;
		GuildsMap mGuilds;

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

	class Client : public Shared::NetworkingWS::Client
	{
	public:
		Client();

	protected:
		std::shared_ptr<Shared::NetworkingWS::Channel> createChannel() override;

	public:
		void onFrame() override;

	public:
		void commit();
		void requestHighscores();
		void requestProfile(int uid);
		void requireProfile(int uid);
		void sendChatMessage(const std::string& text);
		void createGuild(const std::string& title);
		void requestGuildList();
		void exitGuild();
		void joinGuild(int id);

		void requestGuildInfo(int id);
		void requireGuildInfo(int id);
		
	public:
		const Channel::GlobalChatMessages& getGlobalChatMessages() const;

	private:
		auto getMyChannel() const { return std::dynamic_pointer_cast<Channel>(getChannel()); }

	public:
		const Channel::ProfilesMap& getProfiles() const;
		bool hasProfile(int uid);
		Channel::ProfilePtr getProfile(int uid);
		void clearProfiles();

		const Channel::GuildsMap& getGuilds() const;
		bool hasGuild(int uid);
		Channel::GuildPtr getGuild(int uid);
		void clearGuilds();

	public:
		int getUID() const;
	};
}
