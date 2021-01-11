#include "client.h"
#include "helpers.h"

using namespace hcg001;

// channel

Channel::Channel()
{
	addMessageReader("event", [this](auto& buf) {
		auto name = Common::BufferHelpers::ReadString(buf);
		auto params = std::map<std::string, std::string>();
		while (buf.readBit())
		{
			auto key = Common::BufferHelpers::ReadString(buf);
			auto value = Common::BufferHelpers::ReadString(buf);
			params.insert({ key, value });
		}
		if (mEvents.count(name) == 0)
		{
			LOG("unknown event: " + name);
			return;
		}
		mEvents.at(name)(params);
	});

	mEvents["print"] = [](const auto& params) {
		auto text = params.at("text");
		EVENT->emit(Helpers::PrintEvent({ text }));
	};
}

void Channel::sendEvent(const std::string& name, const std::map<std::string, std::string>& params)
{
	auto buf = Common::BitBuffer();
	Common::BufferHelpers::WriteString(buf, name);
	for (auto& [key, value] : params)
	{
		buf.writeBit(true);
		Common::BufferHelpers::WriteString(buf, key);
		Common::BufferHelpers::WriteString(buf, value);
	}
	buf.writeBit(false);
	sendReliable("event", buf);
}

// client

Client::Client() : Shared::Networking::Client({ "192.168.0.106:1337" })
{
	//
}

std::shared_ptr<Shared::Networking::Channel> Client::createChannel()
{
	return std::make_shared<hcg001::Channel>();
}

void Client::sendEvent(const std::string& name, const std::map<std::string, std::string>& params)
{
	if (!isConnected())
		return;

	getMyChannel()->sendEvent(name, params);
}