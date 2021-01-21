#include "client.h"
#include "helpers.h"

using namespace hcg001;

// channel

Channel::Channel()
{
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
    
	auth();
	commit();
	requestHighscores();
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

void Channel::readFileMessage(Common::BitBuffer& buf)
{
	auto path = Common::BufferHelpers::ReadString(buf);
	auto file_size = buf.readBitsVar();
	auto frag_offset = buf.readBitsVar();
	auto frag_size = buf.readBitsVar();

	if (mFiles.count(path) == 0)
	{
		mFiles[path].buf.setSize(file_size);
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
		Platform::Asset::Write(path, file_buf.getMemory(), file_buf.getSize(), Platform::Asset::Storage::Bundle);
		mFiles.erase(path);
		LOG(path + " saved");
	}
}

// client

static bool WantShowChan = false;

Client::Client() : Shared::NetworkingUDP::Client({ "192.168.0.106:1337" })
{
	CONSOLE->registerCVar("hud_show_chan", "show net channel info", { "bool" },
		CVAR_GETTER_BOOL(WantShowChan), CVAR_SETTER_BOOL(WantShowChan));
}

std::shared_ptr<Shared::NetworkingUDP::Channel> Client::createChannel()
{
	return std::make_shared<hcg001::Channel>();
}

void Client::frame()
{
	Shared::NetworkingUDP::Client::frame();

	if (WantShowChan && isConnected())
	{
		STATS_INDICATE_GROUP("chan", "in seq", std::to_string(getChannel()->getIncomingSequence()));
		STATS_INDICATE_GROUP("chan", "in rel_idx", std::to_string(getChannel()->getIncomingReliableIndex()));
	
		STATS_INDICATE_GROUP("chan", "out seq", std::to_string(getChannel()->getOutgoingSequence()));
		STATS_INDICATE_GROUP("chan", "out rel_idx", std::to_string(getChannel()->getOutgoingReliableIndex()));
		
		STATS_INDICATE_GROUP("chan", "rel queue", std::to_string(getChannel()->getOutgoingReliableQueueSize()));
	}
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
