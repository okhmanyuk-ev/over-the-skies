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
	addMessageReader("file", [this](Common::BitBuffer& buf) {
		auto path = Common::BufferHelpers::ReadString(buf);
		auto file_size = buf.readBitsVar();
		auto frag_offset = buf.readBitsVar();
		auto frag_size = buf.readBitsVar();

		if (mFiles.count(path) == 0)
		{
			mFiles[path].first = 0;
			mFiles[path].second.setSize(file_size);
		}

		auto& [progress, file_buf] = mFiles[path];

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

		GAME_STATS(path, std::to_string(int((float)progress / (float)file_size * 100.0f)) + "%%");

		//LOG("path: " + path +
		//	", file_size: " + Common::Helpers::BytesToNiceString(file_size) +
		//	", frag_offset: " + std::to_string(frag_offset) +
		//	", frag_size: " + Common::Helpers::BytesToNiceString(frag_size));

		if (progress == file_size)
		{
			Platform::Asset::Write(path, file_buf.getMemory(), file_buf.getSize());
			mFiles.erase(path);
			LOG(path + " saved");
		}
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