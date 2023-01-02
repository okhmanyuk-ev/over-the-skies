#include "client.h"
#include "helpers.h"

using namespace hcg001;

// channel

Channel::Channel()
{
#ifndef EMSCRIPTEN
	setShowEventLogs(true);

	//addMessageReader("file", [this](auto& buf) { readFileMessage(buf);	});

	addEventCallback("authorized", [this](const auto& json) {
		mUID = json["uid"];

		commit();
		requestHighscores();

		LOG("authorized");
	});

	addEventCallback("guild_id", [](const auto& json) {
		PROFILE->setGuildId(json["id"]);
		LOGF("guild: {}", PROFILE->getGuildId());
	});

	addEventCallback("highscores", [](const auto& json) {
		std::vector<int> highscores = json["highscores"];
		EVENT->emit(NetEvents::HighscoresEvent({ highscores }));
	});

	addEventCallback("guilds_top", [](const auto& json) {
		std::vector<int> guild_ids = json["guild_ids"];
		EVENT->emit(NetEvents::GuildsTopEvent({ guild_ids }));
	});

	addEventCallback("profile", [this](const auto& json) {
		int uid = json["uid"];
		auto profile_json = json["profile"];
		int guild_id = json["guild_id"];
		auto profile = std::make_shared<Profile>();
		profile->read(profile_json);
		profile->setGuildId(guild_id);
		mProfiles.erase(uid);
		mProfiles.insert({ uid, profile });
		EVENT->emit(NetEvents::ProfileReceived({ uid }));
	});

	addEventCallback("global_chat_message", [this](const auto& json) {
		auto msgid = json["msgid"];
		auto uid = json["uid"];
		auto text = json["text"];
		auto message = std::make_shared<ChatMessage>(uid, text);
		mGlobalChatMessages[msgid] = message;
		EVENT->emit(GlobalChatMessageEvent({ msgid }));
	});

	addEventCallback("created_guild", [this](const auto& json) {
		auto e = CreateGuildEvent();

		e.status = json["status"];
		
		if (json.contains("id"))
			e.id = json["id"];
		
		if (e.status == "ok")
			PROFILE->setGuildId(e.id.value());

		EVENT->emit(e);
	});

	addEventCallback("guild_list", [this](const auto& json) {
		std::vector<int> ids = json["ids"];
		EVENT->emit(GuildListEvent({ ids }));
	});

	addEventCallback("guild_info", [this](const auto& json) {
		int id = json["id"];
		std::string title = json["title"];
		std::set<int> members = json["members"];

		auto guild = std::make_shared<Guild>();
		guild->setJson(json);
		mGuilds.erase(id);
		mGuilds.insert({ id, guild });
		EVENT->emit(GuildInfoReceivedEvent({ id }));
	});

	addEventCallback("joined_to_guild", [this](const auto& json) {
		int id = json["id"];
		PROFILE->setGuildId(id);
		EVENT->emit(JoinedToGuildEvent({ id }));
	});

	addEventCallback("exited_from_guild", [this](const auto& json) {
		mGuilds.erase(PROFILE->getGuildId());
		PROFILE->setGuildId(Profile::NoneGuild);
		EVENT->emit(ExitedFromGuildEvent());
	});

	addEventCallback("contributed_to_guild", [this](const auto& json) {
		requestGuildInfo(PROFILE->getGuildId());
	});
	
	FRAME->addOne([this] {
		auth();
	});
#endif
}

void Channel::auth()
{
#ifndef EMSCRIPTEN
#if defined(PLATFORM_WINDOWS)
	auto platform = "win";
#elif defined(PLATFORM_IOS)
	auto platform = "ios";
#elif defined(PLATFORM_ANDROID)
	auto platform = "android";
#elif defined(PLATFORM_MAC)
	auto platform = "mac";
#endif
	sendEvent("auth", {
		{ "platform", platform },
		{ "uuid", PLATFORM->getUUID() } 
	});
#endif
}

void Channel::commit()
{
#ifndef EMSCRIPTEN
	nlohmann::json profile;
	PROFILE->write(profile);

	if (mPrevProfile == profile)
		return;

	mPrevProfile = profile;

	sendEvent("commit", { { "profile", profile } });

	// update profile in map
	mProfiles.erase(mUID);
	mProfiles.insert({ mUID, PROFILE });
	EVENT->emit(NetEvents::ProfileReceived({ mUID }));
#endif
}

void Channel::requestHighscores()
{
#ifndef EMSCRIPTEN
	sendEvent("request_highscores");
	sendEvent("request_guilds_top");
#endif
}

void Channel::requestProfile(int uid)
{
#ifndef EMSCRIPTEN
	if (mProfiles.count(uid) > 0)
		mProfiles.erase(uid);

	sendEvent("request_profile", {
		{ "uid", uid }
	});
#endif
}

void Channel::clearProfiles()
{
	mProfiles.clear();
}

void Channel::sendChatMessage(const std::string& text)
{
#ifndef EMSCRIPTEN
	sendEvent("global_chat_message", {
		{ "text", text }
	});
#endif
}

void Channel::createGuild(const std::string& title)
{
#ifndef EMSCRIPTEN
	sendEvent("create_guild", {
		{ "title", title }
	});
#endif
}

void Channel::requestGuildList()
{
#ifndef EMSCRIPTEN
	sendEvent("request_guild_list");
#endif
}

void Channel::clearGuilds()
{
	mGuilds.clear();
}

void Channel::requestGuildInfo(int id)
{
#ifndef EMSCRIPTEN
	sendEvent("request_guild_info", {
		{ "id", id }
	});
#endif
}

void Channel::exitGuild()
{
#ifndef EMSCRIPTEN
	sendEvent("exit_guild");
#endif
}

void Channel::joinGuild(int id)
{
#ifndef EMSCRIPTEN
	sendEvent("join_guild", {
		{ "id", id }
	});
#endif
}

void Channel::readFileMessage(BitBuffer& buf)
{
	auto path = Common::BufferHelpers::ReadString(buf);
	auto file_size = buf.readBitsVar();
	auto file_crc32 = buf.readBitsVar();
	auto frag_offset = buf.readBitsVar();
	auto frag_size = buf.readBitsVar();

	if (mFiles.count(path) == 0)
	{
		mFiles[path].buf.setSize(file_size);
		mFiles[path].crc32 = file_crc32;
	}

	if (mFiles.at(path).crc32 != file_crc32)
	{
		LOG("file: bad crc32");
		return;
	}

	auto& progress = mFiles[path].progress;
	auto& file_buf = mFiles[path].buf;

	if (file_buf.getSize() != file_size)
	{
		LOG("file: bad file size");
		return;
	}

	if (frag_offset + frag_size > file_buf.getSize())
	{
		LOG("file: bad fragment");
		return;
	}

	for (uint32_t i = frag_offset; i < frag_offset + frag_size; i++)
	{
		((uint8_t*)file_buf.getMemory())[i] = buf.read<uint8_t>();
	}

	progress += frag_size;

	STATS_INDICATE_GROUP("net", path, Common::Helpers::BytesToNiceString(progress) + "/" + Common::Helpers::BytesToNiceString(file_size));

	if (progress == file_size)
	{
		auto crc32 = Common::Helpers::crc32(file_buf.getMemory(), file_buf.getSize());

		if (crc32 == file_crc32)
		{
			Platform::Asset::Write(path, file_buf.getMemory(), file_buf.getSize(), Platform::Asset::Storage::Bundle);
			LOG(path + " saved");
		}
		else
		{
			LOG(path + " not saved (crc32 mismatch)");
		}
		mFiles.erase(path);
	}
}

// channel chat

Channel::ChatMessage::ChatMessage(int uid, const std::string& text) :
	mUID(uid),
	mText(text)
{
	//
}

// client

Client::Client()
#ifndef EMSCRIPTEN
 : 
	//Shared::NetworkingWS::Client("ws://hcg001.ddns.net:27015")
	Shared::NetworkingWS::Client("ws://192.168.0.106:27015")
#endif
{
}

#ifndef EMSCRIPTEN
std::shared_ptr<Shared::NetworkingWS::Channel> Client::createChannel()
{
	return std::make_shared<hcg001::Channel>();
}

void Client::onFrame()
{
	Shared::NetworkingWS::Client::onFrame();
}
#endif

void Client::commit()
{
#ifndef EMSCRIPTEN
	if (!isConnected())
		return;

	getMyChannel()->commit();
#endif
}

void Client::requestHighscores()
{
#ifndef EMSCRIPTEN
	if (!isConnected())
		return;

	getMyChannel()->requestHighscores();
#endif
}

void Client::requestProfile(int uid)
{
#ifndef EMSCRIPTEN
	if (!isConnected())
		return;

	getMyChannel()->requestProfile(uid);
#endif
}

void Client::requireProfile(int uid)
{
#ifndef EMSCRIPTEN
	if (!isConnected())
		return;

	if (getProfiles().count(uid) > 0)
		return;

	requestProfile(uid);
#endif
}

void Client::sendChatMessage(const std::string& text)
{
#ifndef EMSCRIPTEN
	if (!isConnected())
		return;

	getMyChannel()->sendChatMessage(text);
#endif
}

void Client::createGuild(const std::string& title)
{
#ifndef EMSCRIPTEN
	if (!isConnected())
		return;

	getMyChannel()->createGuild(title);
#endif
}

void Client::requestGuildList()
{
#ifndef EMSCRIPTEN
	if (!isConnected())
		return;

	getMyChannel()->requestGuildList();
#endif
}

void Client::exitGuild()
{
#ifndef EMSCRIPTEN
	if (!isConnected())
		return;

	getMyChannel()->exitGuild();
#endif
}

void Client::joinGuild(int id)
{
#ifndef EMSCRIPTEN
	if (!isConnected())
		return;

	getMyChannel()->joinGuild(id);
#endif
}

void Client::requestGuildInfo(int id)
{
#ifndef EMSCRIPTEN
	if (!isConnected())
		return;

	getMyChannel()->requestGuildInfo(id);
#endif
}

void Client::requireGuildInfo(int id)
{
#ifndef EMSCRIPTEN
	if (!isConnected())
		return;

	if (getGuilds().count(id) > 0)
		return;

	requestGuildInfo(id);
#endif
}

void Client::sendGuildContribution(int count)
{
#ifndef EMSCRIPTEN
	if (!isConnected())
		return;

	if (count <= 0)
		return;

	getMyChannel()->sendEvent("guild_contribution", { { "count", count } });
#endif
}

void Client::sendAchievementEarned(const std::string& name)
{
#ifndef EMSCRIPTEN
	if (!isConnected())
		return;

	getMyChannel()->sendEvent("achievement_earned", {
		{ "name", name }
	});
#endif
}

const Channel::GlobalChatMessages& Client::getGlobalChatMessages() const
{
#ifndef EMSCRIPTEN
	assert(isConnected());
	return getMyChannel()->getGlobalChatMessages();
#else
	return {};
#endif
}

const Channel::ProfilesMap& Client::getProfiles() const 
{ 
#ifndef EMSCRIPTEN
	assert(isConnected());
	return getMyChannel()->getProfiles(); 
#else
	return {};
#endif
}

bool Client::hasProfile(int uid)
{
	auto profiles = getProfiles();
	return profiles.count(uid) > 0;
}

Channel::ProfilePtr Client::getProfile(int uid)
{
	assert(hasProfile(uid));
	return getProfiles().at(uid);
}

void Client::clearProfiles()
{
#ifndef EMSCRIPTEN
	if (!isConnected())
		return; 

	getMyChannel()->clearProfiles();
#endif
}

const Channel::GuildsMap& Client::getGuilds() const
{
#ifndef EMSCRIPTEN
	assert(isConnected());
	return getMyChannel()->getGuilds();
#else
	return {};
#endif
}

bool Client::hasGuild(int uid)
{
	return getGuilds().count(uid) > 0;
}

Channel::GuildPtr Client::getGuild(int uid)
{
	assert(hasGuild(uid));
	return getGuilds().at(uid);
}

void Client::clearGuilds()
{
#ifndef EMSCRIPTEN
	if (!isConnected())
		return;

	getMyChannel()->clearGuilds();
#endif
}

int Client::getUID() const
{
#ifndef EMSCRIPTEN
	assert(isConnected());
	return getMyChannel()->getUID();
#else
	return 0;
#endif
}
