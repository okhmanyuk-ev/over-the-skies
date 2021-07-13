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

		sendEvent("authorized", {
			{ "uid", mUID },
		});

		sendGuildId();

		// send last global chat message

		auto msgid = SERVER->getChat().getLastMessageIndex();

		if (msgid == 0) // this means no chat messages
			return;

		auto uid = SERVER->getChat().getMessageUID(msgid);
		auto text = SERVER->getChat().getMessageText(msgid);
		sendGlobalChatMessage(msgid, uid, text);

		// send prev 100 messages (this is fast solution)
		// TODO: add dynamic history requests, and remove this shit. send only one last message
		for (int i = msgid - 1; i > msgid - 100; i--)
		{
			if (i < 1)
				break;

			auto uid = SERVER->getChat().getMessageUID(i);
			auto text = SERVER->getChat().getMessageText(i);
			sendGlobalChatMessage(i, uid, text);
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

	addEventCallback("request_guilds_top", [this](const auto& params) {
		checkAuthorized();
		auto guilds_top = SERVER->getGuildsTop();
		sendEvent("guilds_top", {
			{ "guild_ids", guilds_top }
		});
	});

	addEventCallback("request_profile", [this](const auto& json) {
		checkAuthorized();
		int uid = json["uid"];

		auto profiles = SERVER->getUserbase().getProfiles();

		if (profiles.count(uid) == 0)
			return;

		auto profile_json = profiles.at(uid);

		auto guild_id = SERVER->findUserGuild(uid);

		sendEvent("profile", {
			{ "uid", uid },
			{ "profile", *profile_json },
			{ "guild_id", guild_id.value_or(Guild::NoneGuild) }
		});
	});

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
			{ "members", guild->getMembers() },
			{ "score", guild->getScore() }
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

	addEventCallback("guild_contribution", [this](const auto& json) {
		checkAuthorized();

		int count = json["count"];

		auto guild_id = SERVER->findUserGuild(mUID);

		if (!guild_id.has_value())
			return;

		auto& guilds = SERVER->getGuildsSys().getGuilds();

		if (guilds.count(guild_id.value()) == 0)
			return;

		auto guild = guilds.at(guild_id.value());

		guild->setScore(guild->getScore() + count);

		sendEvent("contributed_to_guild", {
			{ "count", count },
			{ "new_total_score", guild->getScore() }
		});
	});
}

void Channel::log(const std::string& text)
{
	LOGF("#{} {}", mUID, text);
}

void Channel::sendPrint(const std::string& text)
{
	sendEvent("print", { { "text", text } });
}

void Channel::sendGlobalChatMessage(int msgid, int sender_uid, const std::string& text)
{
	sendEvent("global_chat_message", {
		{ "msgid", msgid },
		{ "uid", sender_uid },
		{ "text", text }
	});
}

void Channel::sendGuildId()
{
	checkAuthorized();

	auto guild_id_v = SERVER->findUserGuild(mUID);

	int guild_id = Guild::NoneGuild;

	if (guild_id_v.has_value())
		guild_id = guild_id_v.value();

	sendEvent("guild_id", {
		{ "id", guild_id }
	});
}


void Channel::checkAuthorized()
{
	if (!mAuthorized)
		throw std::runtime_error("not authorized when should be");
}

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
		guild->save();
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
			remakeGuildTop();
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
	remakeGuildTop();
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
		client->sendPrint(text);
	}
	LOG("broadcast message \"" + text + "\" to " + std::to_string(getChannels().size()) + " clients");
}

void Server::broadcastGlobalChatMessage(int msgid, int sender_uid, const std::string& text)
{
	for (auto& [adr, channel] : getChannels())
	{
		auto client = std::dynamic_pointer_cast<Channel>(channel);
		client->sendGlobalChatMessage(msgid, sender_uid, text);
	}
}

void Server::highscore(int uid, int value)
{
	mHighscores[uid] = value;
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

void Server::remakeGuildTop()
{
	auto begin_time = Clock::Now();

	auto guilds_list = mGuilds.getGuildList();
	auto& guilds = mGuilds.getGuilds();

	std::sort(guilds_list.begin(), guilds_list.end(), [&guilds](int left, int right) {
		return guilds.at(left)->getScore() > guilds.at(right)->getScore();
	});

	mSortedGuildTop.clear();

	for (int i = 0; i < 1000; i++)
	{
		if (guilds_list.size() - 1 < i)
			break;

		mSortedGuildTop.push_back(guilds_list.at(i));
	}

	auto end_time = Clock::Now();

	LOGF("remaked guild top, duration: {} msec", Clock::ToMilliseconds(end_time - begin_time));
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
