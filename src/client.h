#pragma once

#include <shared/all.h>

#define CLIENT ENGINE->getSystem<hcg001::Client>()

namespace hcg001
{
	class Channel : public Shared::NetworkingUDP::SimpleChannel
	{
	public:
		Channel();

	private:
		void readFileMessage(Common::BitBuffer& buf);

	private:
		struct FileDef
		{
			size_t progress = 0;
			Common::BitBuffer buf;
		};
		std::map<std::string, FileDef> mFiles;
	};

	class Client : public Shared::NetworkingUDP::Client
	{
	public:
		Client();

	protected:
		std::shared_ptr<Shared::NetworkingUDP::Channel> createChannel() override;

	public:
		void sendEvent(const std::string& name, const std::map<std::string, std::string>& params);

	private:
		auto getMyChannel() const { return std::dynamic_pointer_cast<Channel>(getChannel()); }
	};
}
