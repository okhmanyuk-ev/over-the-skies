#pragma once

#include <shared/all.h>

#define CLIENT ENGINE->getSystem<Client>()

namespace hcg001
{
	class Client : public Shared::Networking::Client
	{
	public:
		Client();

	protected:
		void onEvent(const std::string& name, const std::map<std::string, std::string>& params) override;
	};
}
