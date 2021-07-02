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
	void print(const std::string& text);
	void globalChatMessage(int msgid, int sender_uid, const std::string& text);

private:
	void checkAuthorized();

private:
	bool mAuthorized = false;
	Shared::NetworkingWS::Userbase::UID mUID = 0;
	std::shared_ptr<Shared::NetworkingWS::Userbase::Profile> mProfile = nullptr;
};

/*class Database
{
public:
	static inline const int NoneGuild = -1;

public:
	Database();

private:
	void check();

public:
	bool isUserExists(int uid);
	bool isUserExists(const std::string& platform, const std::string& uuid);
	void registerUser(const std::string& platform, const std::string& uuid);
	int getUsersCount();

	// returning id
	int auth(const std::string& platform, const std::string& uuid);

	void profile(int uid, const std::string& data);
	void highscore(int uid, int value);
	int getUserGuild(int uid);
	void setUserGuild(int user_id, int guild_id);

	// return id array
	std::vector<int> getHighscores();

	// return json dump
	std::string getProfile(int uid);

	// return msgid
	int addMessageToGlobalChat(int uid, const std::string& text);
	int getLastGlobalChatMessageIndex();
	int getGlobalChatMessageUID(int msgid);
	std::string getGlobalChatMessageText(int msgid);
};*/

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
	void joinToGuild(int guild_id, int user_id);
	void exitFromGuild(int guild_id, int user_id);
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
};
