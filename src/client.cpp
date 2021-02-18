#include "client.h"
#include "helpers.h"

using namespace hcg001;

// channel

Channel::Channel()
{
	setShowEventLogs(true);

	addMessageReader("file", [this](auto& buf) { readFileMessage(buf);	});

	addEventCallback("authorized", [this](const auto& params) {
		mUID = std::stoi(params.at("uid"));
		LOG("authorized (uid: " + std::to_string(mUID) + ")");
	});

	addEventCallback("print", [](const auto& params) {
		auto text = params.at("text");
		EVENT->emit(Helpers::PrintEvent({ text }));
	});

	addEventCallback("highscores", [](const auto& params) {
		nlohmann::json json = nlohmann::json::parse(params.at("json"));
        auto uids = json.get<std::vector<int>>();
		EVENT->emit(Helpers::HighscoresEvent({ uids }));
	});

	addEventCallback("profile", [this](const auto& params) {
		auto uid = std::stoi(params.at("uid"));
		auto dump = params.at("json");
		auto json = nlohmann::json::parse(dump);
		auto profile = std::make_shared<Profile>();
		profile->read(json);
		mProfiles.erase(uid);
		mProfiles.insert({ uid, profile });
		EVENT->emit(Helpers::ProfileReceived({ uid }));
	});

	addEventCallback("global_chat_message", [this](const auto& params) {
		auto msgid = std::stoi(params.at("msgid"));
		auto uid = std::stoi(params.at("uid"));
		auto text = params.at("text");
		auto message = std::make_shared<ChatMessage>(uid, text);
		mGlobalChatMessages[msgid] = message;
		EVENT->emit(GlobalChatMessageEvent({ msgid }));
	});

	addEventCallback("create_guild", [this](const auto& params) {
		auto status = params.at("status");
		EVENT->emit(CreateGuildEvent({ status }));
	});

	addEventCallback("guild_list", [this](const auto& params) {
		auto dump = params.at("ids");
		auto json = nlohmann::json::parse(dump);
		std::vector<int> ids = json;
		EVENT->emit(GuildListEvent({ ids }));
	});

	addEventCallback("guild_info", [this](const auto& params) {
		auto id = std::stoi(params.at("id"));
		auto dump = params.at("json");
		auto json = nlohmann::json::parse(dump);
		auto guild = std::make_shared<Guild>();
		guild->setJson(json);
		mGuilds.erase(id);
		mGuilds.insert({ id, guild });
		EVENT->emit(GuildInfoReceivedEvent({ id }));
	});

	FRAME->addOne([this] {
		auth();
		commit();
		requestHighscores();
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

	auto dump = profile.dump();

	if (mPrevProfileDump == dump)
		return;

	mPrevProfileDump = dump;

	sendEvent("commit", { { "profile", dump,  } });

	// update profile in map
	mProfiles.erase(mUID);
	mProfiles.insert({ mUID, PROFILE });
	EVENT->emit(Helpers::ProfileReceived({ mUID }));
}

void Channel::requestHighscores()
{
	sendEvent("request_highscores");
}

void Channel::requestProfile(int uid)
{
	if (mProfiles.count(uid) > 0)
		mProfiles.erase(uid);

	sendEvent("request_profile", {
		{ "uid", std::to_string(uid) }
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
		{ "id", std::to_string(id) }
	});
}

void Channel::readFileMessage(Common::BitBuffer& buf)
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
	//
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
	return getProfiles().count(uid) > 0;
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
	assert(hasProfile(uid));
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