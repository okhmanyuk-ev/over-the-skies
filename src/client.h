#pragma once

#include <shared/all.h>

#define CLIENT ENGINE->getSystem<hcg001::Client>()

namespace hcg001
{
	class Channel : public Shared::NetworkingUDP::SimpleChannel
	{
	public:
		Channel();

	public:
		void auth();
		void commit();
		void requestHighscores();
		void requestProfile(int uid);

	private:
		void readFileMessage(Common::BitBuffer& buf);

	private:
		struct FileDef
		{
			size_t progress = 0;
			Common::BitBuffer buf;
		};
		std::map<std::string, FileDef> mFiles;

	private:
		std::string mPrevProfileDump;
	};

	class Client : public Shared::NetworkingUDP::Client
	{
	public:
		static inline std::map<int/*id*/, nlohmann::json> Profiles;

	public:
		Client();

	protected:
		std::shared_ptr<Shared::NetworkingUDP::Channel> createChannel() override;

	public:
		void commit();
		void requestHighscores();
		void requestProfile(int uid);

	private:
		auto getMyChannel() const { return std::dynamic_pointer_cast<Channel>(getChannel()); }
	};
}
