#pragma once

#include <shared/all.h>

#define CLIENT ENGINE->getSystem<hcg001::Client>()

namespace hcg001
{
	class Client : public Shared::Networking::Client
	{
	public:
		void sendEvent(const std::string& name, const std::map<std::string, std::string>& params);
	};
}
