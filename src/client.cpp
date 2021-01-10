#include "client.h"

using namespace hcg001;

Client::Client() : Shared::Networking::Client({ "127.0.0.1:1337" })
{
	//
}

void Client::onEvent(const std::string& name, const std::map<std::string, std::string>& params)
{
	LOG("event received: " + name);
}