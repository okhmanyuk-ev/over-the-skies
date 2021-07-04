#include "server.h"

// channel

Channel::Channel()
{
	setShowEventLogs(true);

	addEventCallback("auth", [this](const auto& json) {
		auto platform = json["platform"];
		auto uuid = json["uuid"];

		mAuthorized = true;

		std::tie(mUID, mProfile) = SERVER->getUserbase().auth(platform, uuid);

		log("authorized");

		auto guild_id = SERVER->findUserGuild(mUID);

		sendEvent("authorized", {
			{ "uid", mUID },
		//	{ "guild_id", std::to_string(guild_id) }
		});

		/*auto guild_id = SERVER->getDatabase().getUserGuild(mUID);

		if (!SERVER->getGuilds().count(guild_id))
			guild_id = Database::NoneGuild;

		log("authorized");

		sendEvent("authorized", {
			{ "uid", std::to_string(mUID) },
			{ "guild_id", std::to_string(guild_id) }
		});*/

		// send last global chat message

		auto msgid = SERVER->getChat().getLastMessageIndex();

		if (msgid == 0) // this means no chat messages
			return;

		auto uid = SERVER->getChat().getMessageUID(msgid);
		auto text = SERVER->getChat().getMessageText(msgid);
		globalChatMessage(msgid, uid, text);

		// send prev 100 messages (this is fast solution)
		for (int i = msgid - 1; i > msgid - 100; i--)
		{
			if (i < 1)
				break;

			auto uid = SERVER->getChat().getMessageUID(i);
			auto text = SERVER->getChat().getMessageText(i);
			globalChatMessage(i, uid, text);
		}
	});

	addEventCallback("commit", [this](const auto& json) {
		checkAuthorized();
		log("commit received");

		auto profile = json["profile"];

		SERVER->getUserbase().commit(mUID, std::make_shared<nlohmann::json>(profile));
		SERVER->highscore(mUID, profile["highscore"]);
	});

	addEventCallback("request_highscores", [this](const auto& params) {
		checkAuthorized();
		auto highscores = SERVER->getHighscores();
		sendEvent("highscores", {
			{ "highscores", highscores }
		});
	});

	/*addEventCallback("request_profile", [this](const auto& params) {
		checkAuthorized();
		auto uid_s = params.at("uid");
		auto uid = std::stoi(uid_s);
		auto dump = SERVER->getDatabase().getProfile(uid);
		auto guild_id = SERVER->getDatabase().getUserGuild(uid);
		sendEvent("profile", {
			{ "uid", uid_s },
			{ "json", dump },
			{ "guild_id", std::to_string(guild_id) }
		});
	});*/

	addEventCallback("global_chat_message", [this](const auto& json) {
		checkAuthorized();
		std::string text = json["text"];
		auto msgid = SERVER->getChat().addMessage(mUID, text);
		SERVER->broadcastGlobalChatMessage(msgid, mUID, text);
		log("write to chat \"" + text + "\"");
	});

	addEventCallback("create_guild", [this](const auto& json) {
		checkAuthorized();

		std::string title = json["title"];

		if (SERVER->getGuildsSys().isGuildExist(title))
		{
			sendEvent("created_guild", {
				{ "status", "failed" },
				{ "reason", "name already used" }
			});
			
			return;
		}

		auto guild_id = SERVER->getGuildsSys().createGuild(title);
		SERVER->joinToGuild(guild_id, mUID);

		log("created guild #" + std::to_string(guild_id));

		sendEvent("created_guild", {
			{ "status", "ok" },
			{ "id", guild_id }
		});
	});

	addEventCallback("request_guild_list", [this](const auto& json) {
		checkAuthorized();

		auto guild_list = SERVER->getGuildsSys().getGuildList();

		sendEvent("guild_list", {
			{ "ids", guild_list }
		});
	});

	addEventCallback("request_guild_info", [this](const auto& json) {
		checkAuthorized();

		int id = json["id"];
		auto guild = SERVER->getGuildsSys().getGuilds().at(id);

		sendEvent("guild_info", {
			{ "id", id },
			{ "title", guild->getName() },
			{ "members", guild->getMembers() }
		});
	});
	
	addEventCallback("exit_guild", [this](const auto& json) {
		checkAuthorized();
		auto guild_id = SERVER->findUserGuild(mUID);
		if (!guild_id.has_value())
		{
			LOG("cannot exit_guild, not in guild");
			return;
		}
		SERVER->exitFromGuild(guild_id.value(), mUID);
		sendEvent("exited_from_guild");
		LOGF("#{} exited from guild #{}", mUID, guild_id.value());
	});

	addEventCallback("join_guild", [this](const auto& json) {
		checkAuthorized();
		
		int id = json["id"];
		SERVER->joinToGuild(id, mUID);

		sendEvent("joined_to_guild", {
			{ "id", id }
		});

		LOGF("#{} joined to guild #{}", mUID, id);
	});
}

void Channel::log(const std::string& text)
{
	LOGF("#{} {}", mUID, text);
}

void Channel::print(const std::string& text)
{
	sendEvent("print", { { "text", text } });
}

void Channel::globalChatMessage(int msgid, int sender_uid, const std::string& text)
{
	sendEvent("global_chat_message", {
		{ "msgid", msgid },
		{ "uid", sender_uid },
		{ "text", text }
	});
}

void Channel::checkAuthorized()
{
	if (!mAuthorized)
		throw std::runtime_error("not authorized when should be");
}

// database

/*Database::Database()
{
	mDatabase.execute("CREATE TABLE IF NOT EXISTS users(\
		id INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT,\
		win_uuid VARCHAR(64),\
		ios_uuid VARCHAR(64),\
		android_uuid VARCHAR(64),\
		profile TEXT,\
		highscore INTEGER,\
		guild_id INTEGER\
	);");

	check();
	LOGF("{} users loaded", getUsersCount());

	mDatabase.execute("CREATE TABLE IF NOT EXISTS global_chat(\
		id INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT,\
		uid INTEGER NOT NULL,\
		text TEXT\
	);");

	check();
}

void Database::check()
{
	auto err = mDatabase.error_code();
	if (err != SQLITE_OK && err != SQLITE_DONE)
	{
		auto errmsg = mDatabase.error_msg();
		throw std::runtime_error(errmsg);
	}
}

bool Database::isUserExists(int uid)
{
	auto sql = "SELECT * FROM users WHERE id='" + std::to_string(uid) + "';";
	sqlite3pp::query query(mDatabase, sql.c_str());
	check();
	auto count = std::distance(query.begin(), query.end());
	return count > 0;
}

bool Database::isUserExists(const std::string& platform, const std::string& uuid)
{
	auto sql = "SELECT * FROM users WHERE " + platform + "_uuid='" + uuid + "';";
	sqlite3pp::query query(mDatabase, sql.c_str());
	check();
	auto count = std::distance(query.begin(), query.end());
	return count > 0;
}

void Database::registerUser(const std::string& platform, const std::string& uuid)
{
	assert(!isUserExists(platform, uuid));
	auto sql = "INSERT INTO users (" + platform + "_uuid) VALUES (?)";
	sqlite3pp::command cmd(mDatabase, sql.c_str());
	cmd.binder() << uuid;
	cmd.execute();
	check();
	LOG("registered");
}

int Database::getUserGuild(int uid)
{
	assert(isUserExists(uid));
	auto sql = "SELECT guild_id FROM users WHERE id=" + std::to_string(uid) + ";";
	sqlite3pp::query query(mDatabase, sql.c_str());
	check();
	for (auto column : query)
	{
		if (column.data_count() != 1)
			throw std::runtime_error("getUserGuild: should be 1 row");

		return column.get<int>(0);
	}
	throw std::runtime_error("getUserGuild: wtf");
}

void Database::setUserGuild(int user_id, int guild_id)
{
	assert(isUserExists(user_id));
	
	//if (guild_id != NoneGuild)
	//	assert(isGuildExists(guild_id));
	
	auto sql = "UPDATE users SET (guild_id) = (?) WHERE id=" + std::to_string(user_id) + ";";
	sqlite3pp::command cmd(mDatabase, sql.c_str());
	cmd.binder() << guild_id;
	cmd.execute();
	check();
}

int Database::getUsersCount()
{
	auto sql = "SELECT COUNT(*) FROM users;";
	sqlite3pp::query query(mDatabase, sql);
	check();

	for (auto column : query)
	{
		return column.get<int>(0);
	}

	return -1;
}

int Database::auth(const std::string& platform, const std::string& uuid)
{
	bool was_registered = false;

	if (!isUserExists(platform, uuid))
	{
		was_registered = true;
		registerUser(platform, uuid);
	}

	auto sql = "SELECT id FROM users WHERE " + platform + "_uuid='" + uuid + "';";
	sqlite3pp::query query(mDatabase, sql.c_str());
	check();
	for (auto column : query)
	{
		if (column.data_count() != 1)
			throw std::runtime_error("db_auth: should be 1 row");
		
		auto uid = column.get<int>(0);

		if (was_registered)
			setUserGuild(uid, NoneGuild);

		return uid;
	}
	throw std::runtime_error("db_auth: no unique id found");
}

void Database::profile(int uid, const std::string& data)
{
	assert(isUserExists(uid));
	auto sql = "UPDATE users SET (profile) = (?) WHERE id=" + std::to_string(uid) + ";";
	sqlite3pp::command cmd(mDatabase, sql.c_str());
	cmd.binder() << data;
	cmd.execute();
	check();
}

void Database::highscore(int uid, int value)
{
	assert(isUserExists(uid));
	auto sql = "UPDATE users SET (highscore) = (?) WHERE id=" + std::to_string(uid) + ";";
	sqlite3pp::command cmd(mDatabase, sql.c_str());
	cmd.binder() << value;
	cmd.execute();
	check();
}

std::vector<int> Database::getHighscores()
{
	auto sql = "SELECT id FROM users ORDER BY highscore DESC;"; // TODO: limit 
	sqlite3pp::query query(mDatabase, sql);
	check();

	std::vector<int> result;

	for (auto column : query)
	{
		for (int i = 0; i < column.data_count(); i++)
		{
			auto id = column.get<int>(i);
			result.push_back(id);
		}
	}

	return result;
}

std::string Database::getProfile(int uid)
{
	assert(isUserExists(uid));
	auto sql = "SELECT profile FROM users WHERE id=" + std::to_string(uid) + ";";
	sqlite3pp::query query(mDatabase, sql.c_str());
	check();
	for (auto column : query)
	{
		if (column.data_count() != 1)
			throw std::runtime_error("db_getProfile: should be 1 row");

		return column.get<std::string>(0);
	}
	throw std::runtime_error("db_getProfile: not found");
}

int Database::addMessageToGlobalChat(int uid, const std::string& text)
{
	if (!isUserExists(uid))
		throw std::runtime_error("[Database::addMessageToGlobalChat] user " + std::to_string(uid) + " does not exist");
	
	auto sql = "INSERT INTO global_chat (uid, text) VALUES (?, ?)";
	sqlite3pp::command cmd(mDatabase, sql);
	cmd.binder() << uid << text;
	cmd.execute();
	check();
	return getLastGlobalChatMessageIndex();
}

int Database::getLastGlobalChatMessageIndex()
{
	sqlite3pp::query query(mDatabase, "SELECT MAX(id) FROM global_chat");
	check();
	for (auto column : query)
	{
		if (column.data_count() != 1)
			throw std::runtime_error("[Database::getLastGlobalChatMessageIndex] should be 1 row");

		return column.get<int>(0);
	}
	throw std::runtime_error("[Database::getLastGlobalChatMessageIndex] wtf");
}

int Database::getGlobalChatMessageUID(int msgid)
{
	auto sql = "SELECT uid FROM global_chat WHERE id=" + std::to_string(msgid) + ";";
	sqlite3pp::query query(mDatabase, sql.c_str());
	check();
	for (auto column : query)
	{
		if (column.data_count() != 1)
			throw std::runtime_error("[Database::getGlobalChatMessageUID] should be 1 row");

		return column.get<int>(0);
	}
	throw std::runtime_error("[Database::getGlobalChatMessageUID] not found");
}

std::string Database::getGlobalChatMessageText(int msgid)
{
	auto sql = "SELECT text FROM global_chat WHERE id=" + std::to_string(msgid) + ";";
	sqlite3pp::query query(mDatabase, sql.c_str());
	check();
	for (auto column : query)
	{
		if (column.data_count() != 1)
			throw std::runtime_error("[Database::getGlobalChatMessageText] should be 1 row");

		return column.get<std::string>(0);
	}
	throw std::runtime_error("[Database::getGlobalChatMessageText] not found");
}*/

// chat

int Chat::addMessage(int uid, const std::string& text)
{
	mLastIndex += 1;

	nlohmann::json json = {
		{ "uid", uid },
		{ "text", text }
	};

	mMessages.insert({ mLastIndex, json });

	return getLastMessageIndex();
}

int Chat::getLastMessageIndex()
{
	return mLastIndex;
}
int Chat::getMessageUID(int msgid)
{
	return mMessages.at(msgid)["uid"];
}
std::string Chat::getMessageText(int msgid)
{
	return mMessages.at(msgid)["text"];
}

void Chat::save(nlohmann::json& json)
{
	auto& chat = json["chat"];

	chat["last_index"] = mLastIndex;

	auto& messages = chat["messages"];

	for (const auto& [index, msg] : mMessages)
	{
		nlohmann::json message = {
			{ "index", index },
			{ "msg", msg }
		};

		messages.push_back(message);
	}
}

void Chat::load(const nlohmann::json& json)
{
	auto& chat = json["chat"];

	mLastIndex = chat["last_index"];

	for (const auto& message : chat["messages"])
	{
		int index = message["index"];
		auto msg = message["msg"];
		
		mMessages.insert({ index, msg });
	}
}

// guilds

bool Guilds::isGuildExist(const std::string& title) const
{
	for (auto [index, guild] : mGuilds)
	{
		if (guild->getName() != title)
			continue;

		return true;
	}

	return false;
}

int Guilds::createGuild(const std::string& title)
{
	assert(!isGuildExist(title));

	auto guild = std::make_shared<Guild>(mGuildIndex);
	guild->setName(title);
	guild->save();

	mGuilds[mGuildIndex] = guild;
	mGuildIndex += 1;

	SERVER->save();

	return mGuildIndex - 1;
}

std::vector<int> Guilds::getGuildList() const
{
	std::vector<int> result;

	for (auto [index, guild] : mGuilds)
	{
		result.push_back(index);
	}

	return result;
}

void Guilds::save(nlohmann::json& json)
{
	std::set<int> guilds;

	for (auto [index, guild] : mGuilds)
	{
		guilds.insert(index);
	}
	json["guilds"] = guilds;
	json["guild_index"] = mGuildIndex;
}

void Guilds::load(const nlohmann::json& json)
{
	auto guilds = json["guilds"].get<std::set<int>>();

	for (auto index : guilds)
	{
		auto guild = std::make_shared<Guild>(index);
		guild->load();
		mGuilds[index] = guild;
	}

	if (json.contains("guild_index"))
		mGuildIndex = json["guild_index"];
}

// server

Server::Server() : Shared::NetworkingWS::Server(27015)
{
	load();

	Actions::Run(Actions::Collection::RepeatInfinite([this] {
		const auto Delay = 60.0f; // one minute
		return Actions::Collection::Delayed(Delay, Actions::Collection::Execute([this] {
			remakeHighscores();
			save();
		}));
	}));
}

void Server::save()
{
	auto json = nlohmann::json();
	mUserbase.save(json);
	mChat.save(json);
	mGuilds.save(json);
	auto dump = json.dump(1);
	Platform::Asset::Write("server.json", dump.data(), dump.size(), Platform::Asset::Storage::Absolute);
	LOG("saved");
}

void Server::load()
{
	auto path = "server.json";

	if (!Platform::Asset::Exists(path, Platform::Asset::Storage::Absolute))
		return;

	auto json_file = Platform::Asset(path, Platform::Asset::Storage::Absolute);
	auto json_string = std::string((char*)json_file.getMemory(), json_file.getSize());
	auto json = nlohmann::json::parse(json_string);

	mUserbase.load(json);
	mChat.load(json);
	mGuilds.load(json);

	// collect highscores from profiles

	for (const auto& [uid, profile] : mUserbase.getProfiles())
	{
		highscore(uid, (*profile)["highscore"]);
	}

	remakeHighscores();
}

std::shared_ptr<Shared::NetworkingWS::Channel> Server::createChannel()
{
	return std::make_shared<Channel>();
}

void Server::broadcastPrint(const std::string& text)
{
	for (auto& [adr, channel] : getChannels())
	{
		auto client = std::dynamic_pointer_cast<Channel>(channel);
		client->print(text);
	}
	LOG("broadcast message \"" + text + "\" to " + std::to_string(getChannels().size()) + " clients");
}

void Server::broadcastGlobalChatMessage(int msgid, int sender_uid, const std::string& text)
{
	for (auto& [adr, channel] : getChannels())
	{
		auto client = std::dynamic_pointer_cast<Channel>(channel);
		client->globalChatMessage(msgid, sender_uid, text);
	}
}

void Server::highscore(int uid, int value)
{
	mHighscores[uid] = value;
}

std::vector<int> Server::getHighscores()
{
	return mSortedHighscores;
}

void Server::remakeHighscores()
{
	auto begin_time = Clock::Now();

	struct def
	{
		int uid;
		int score;
	};

	std::vector<def> defs;

	for (const auto& [uid, score] : mHighscores)
	{
		defs.push_back({ uid, score });
	}

	std::sort(defs.begin(), defs.end(), [](const def& _def1, const def& _def2) {
		return _def1.score < _def2.score;
	});

	mSortedHighscores.clear();

	for (int i = 0; i < 1000; i++)
	{
		if (defs.size() - 1 < i)
			break;

		mSortedHighscores.push_back(defs[i].uid);
	}

	auto end_time = Clock::Now();

	LOGF("remaked highscores, duration: {} msec", Clock::ToMilliseconds(end_time - begin_time));
}

void Server::joinToGuild(int guild_id, int uid)
{
	auto& guild = mGuilds.getGuilds().at(guild_id);
	guild->getMembers().insert(uid);
}

void Server::exitFromGuild(int guild_id, int uid)
{
	auto& guild = mGuilds.getGuilds().at(guild_id);
	guild->getMembers().erase(uid);
}

std::optional<int> Server::findUserGuild(int uid)
{
	const auto& guilds = mGuilds.getGuilds();

	for (const auto& [guild_id, guild] : guilds)
	{
		const auto& members = guild->getMembers();

		if (members.count(uid) == 0)
			continue;

		return guild_id;
	}

	return std::nullopt;
}
