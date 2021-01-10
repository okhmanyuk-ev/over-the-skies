#include "client.h"
#include "helpers.h"

using namespace hcg001;

Client::Client() : Shared::Networking::Client({ "192.168.0.106:1337" })
{
	mEvents["print"] = [](const auto& params) {
		auto text = params.at("text");
		EVENT->emit(Helpers::PrintEvent({ text }));
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