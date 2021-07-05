#pragma once

#include <shared/all.h>
#include "guild.h"

#define SERVER ENGINE->getSystem<Server>()

class Channel : public Shared::NetworkingWS::SimpleChannel
{
public:
	Channel();

public:
	void log(const std::string& text);
	void sendPrint(const std::string& text);
	void sendGlobalChatMessage(int msgid, int sender_uid, const std::string& text);
	void sendGuildId();

private:
	void checkAuthorized();

private:
	bool mAuthorized = false;
	Shared::NetworkingWS::Userbase::UID mUID = 0;
	std::shared_ptr<Shared::NetworkingWS::Userbase::Profile> mProfile = nullptr;
};

class Chat // global chat
{
public:
	int addMessage(int uid, const std::string& text);
	int getLastMessageIndex();
	int getMessageUID(int msgid);
	std::string getMessageText(int msgid);

public:
	void save(nlohmann::json& json);
	void load(const nlohmann::json& json);

private:
	std::map<int, nlohmann::json> mMessages;
	int mLastIndex = 0;
};

class Guilds
{
public:
	bool isGuildExist(const std::string& title) const;
	int createGuild(const std::string& title);
	std::vector<int> getGuildList() const;
	const auto& getGuilds() const { return mGuilds; }

private:
	std::map<int, std::shared_ptr<Guild>> mGuilds;
	int mGuildIndex = 0;

public:
	void save(nlohmann::json& json);
	void load(const nlohmann::json& json);
};

class Server : public Shared::NetworkingWS::Server
{
public:
	Server();

	void save();
	void load();

	std::shared_ptr<Shared::NetworkingWS::Channel> createChannel() override;
	void broadcastPrint(const std::string& text);
	void broadcastGlobalChatMessage(int msgid, int sender_uid, const std::string& text);

public:
	auto& getUserbase() { return mUserbase; }
	auto& getChat() { return mChat; }
	auto& getGuildsSys() { return mGuilds; }

private:
	Shared::NetworkingWS::Userbase mUserbase;
	Chat mChat;
	Guilds mGuilds;

public: // highscores
	void highscore(int uid, int value);
	// return uid array
	std::vector<int> getHighscores();

	std::unordered_map<int, int> mHighscores; // uid, highscore
	std::vector<int> mSortedHighscores;
	void remakeHighscores();

	void joinToGuild(int guild_id, int uid);
	void exitFromGuild(int guild_id, int uid);
	std::optional<int> findUserGuild(int uid);
};
