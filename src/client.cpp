#include "client.h"

using namespace hcg001;

Client::Client() : Shared::Networking::Client({ "127.0.0.1:1337" })
{
	mEvents["print"] = [](auto& params) {
		LOG(params.at("text"));
	};
}

void Client::onEvent(const std::string& name, const std::map<std::string, std::string>& params)
{
	if (mEvents.count(name) == 0)
	{
		LOG("unknown event: " + name);
		return;
	}

	mEvents.at(name)(params);
}