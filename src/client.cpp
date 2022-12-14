#include "client.h"
#include "helpers.h"

using namespace hcg001;

// channel

Channel::Channel()
{
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
}

void Channel::auth()
{
#if defined(PLATFORM_WINDOWS)
	auto platform = "win";
#elif defined(PLATFORM_IOS)
	auto platform = "ios";
#elif defined(PLATFORM_ANDROID)
	auto platform = "android";
#elif defined(PLATFORM_MAC)
	auto platform = "mac";
#elif defined(PLATFORM_EMSCRIPTEN)
	auto platform = "emscripten";
#endif
	sendEvent("auth", {
		{ "platform", platform },
		{ "uuid", PLATFORM->getUUID() } 
	});
}

void Channel::commit()
{
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
}

void Channel::requestHighscores()
{
	sendEvent("request_highscores");
	sendEvent("request_guilds_top");
}

void Channel::requestProfile(int uid)
{
	if (mProfiles.count(uid) > 0)
		mProfiles.erase(uid);

	sendEvent("request_profile", {
		{ "uid", uid }
	});
}

void Channel::clearProfiles()
{
	mProfiles.clear();
}

void Channel::sendChatMessage(const std::string& text)
{
	sendEvent("global_chat_message", {
		{ "text", text }
	});
}

void Channel::createGuild(const std::string& title)
{
	sendEvent("create_guild", {
		{ "title", title }
	});
}

void Channel::requestGuildList()
{
	sendEvent("request_guild_list");
}

void Channel::clearGuilds()
{
	mGuilds.clear();
}

void Channel::requestGuildInfo(int id)
{
	sendEvent("request_guild_info", {
		{ "id", id }
	});
}

void Channel::exitGuild()
{
	sendEvent("exit_guild");
}

void Channel::joinGuild(int id)
{
	sendEvent("join_guild", {
		{ "id", id }
	});
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

Client::Client() : 
	//Shared::NetworkingWS::Client("ws://hcg001.ddns.net:27015")
	Shared::NetworkingWS::Client("ws://192.168.0.106:27015")
{
}

std::shared_ptr<Shared::NetworkingWS::Channel> Client::createChannel()
{
	return std::make_shared<hcg001::Channel>();
}

void Client::onFrame()
{
	Shared::NetworkingWS::Client::onFrame();
}

void Client::commit()
{
	if (!isConnected())
		return;

	getMyChannel()->commit();
}

void Client::requestHighscores()
{
	if (!isConnected())
		return;

	getMyChannel()->requestHighscores();
}

void Client::requestProfile(int uid)
{
	if (!isConnected())
		return;

	getMyChannel()->requestProfile(uid);
}

void Client::requireProfile(int uid)
{
	if (!isConnected())
		return;

	if (getProfiles().count(uid) > 0)
		return;

	requestProfile(uid);
}

void Client::sendChatMessage(const std::string& text)
{
	if (!isConnected())
		return;

	getMyChannel()->sendChatMessage(text);
}

void Client::createGuild(const std::string& title)
{
	if (!isConnected())
		return;

	getMyChannel()->createGuild(title);
}

void Client::requestGuildList()
{
	if (!isConnected())
		return;

	getMyChannel()->requestGuildList();
}

void Client::exitGuild()
{
	if (!isConnected())
		return;

	getMyChannel()->exitGuild();
}

void Client::joinGuild(int id)
{
	if (!isConnected())
		return;

	getMyChannel()->joinGuild(id);
}

void Client::requestGuildInfo(int id)
{
	if (!isConnected())
		return;

	getMyChannel()->requestGuildInfo(id);
}

void Client::requireGuildInfo(int id)
{
	if (!isConnected())
		return;

	if (getGuilds().count(id) > 0)
		return;

	requestGuildInfo(id);
}

void Client::sendGuildContribution(int count)
{
	if (!isConnected())
		return;

	if (count <= 0)
		return;

	getMyChannel()->sendEvent("guild_contribution", { { "count", count } });
}

void Client::sendAchievementEarned(const std::string& name)
{
	if (!isConnected())
		return;

	getMyChannel()->sendEvent("achievement_earned", {
		{ "name", name }
	});
}

const Channel::GlobalChatMessages& Client::getGlobalChatMessages() const
{
	assert(isConnected());
	return getMyChannel()->getGlobalChatMessages();
}

const Channel::ProfilesMap& Client::getProfiles() const 
{ 
	assert(isConnected());
	return getMyChannel()->getProfiles(); 
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
	if (!isConnected())
		return; 

	getMyChannel()->clearProfiles();
}

const Channel::GuildsMap& Client::getGuilds() const
{
	assert(isConnected());
	return getMyChannel()->getGuilds();
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
	if (!isConnected())
		return;

	getMyChannel()->clearGuilds();
}

int Client::getUID() const
{
	assert(isConnected());
	return getMyChannel()->getUID();
}
