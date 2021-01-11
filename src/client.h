#pragma once

#include <shared/all.h>

#define CLIENT ENGINE->getSystem<hcg001::Client>()

namespace hcg001
{
	class Channel : public Shared::Networking::Channel
	{
	public:
		Channel();

	public:
		void sendEvent(const std::string& name, const std::map<std::string, std::string>& params);

	private:
		std::map<std::string, std::function<void(std::map<std::string, std::string>)>> mEvents;
		std::map<std::string, std::pair<size_t/*progress*/, Common::BitBuffer>> mFiles;
	};

	class Client : public Shared::Networking::Client
	{
	public:
		Client();

	protected:
		std::shared_ptr<Shared::Networking::Channel> createChannel() override;

	public:
		void sendEvent(const std::string& name, const std::map<std::string, std::string>& params);

	private:
		auto getMyChannel() const { return std::dynamic_pointer_cast<Channel>(getChannel()); }
	};
}
