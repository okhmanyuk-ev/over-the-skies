#include "client.h"
#include "helpers.h"

using namespace hcg001;

// channel

Channel::Channel()
{
	addMessageReader("file", [this](auto& buf) { readFileMessage(buf);	});
	addEventCallback("print", [](const auto& params) {
		auto text = params.at("text");
		EVENT->emit(Helpers::PrintEvent({ text }));
	});
    
    sendEvent("auth", { { "uuid", PLATFORM->getUUID() } });
}

void Channel::readFileMessage(Common::BitBuffer& buf)
{
	auto path = Common::BufferHelpers::ReadString(buf);
	auto file_size = buf.readBitsVar();
	auto frag_offset = buf.readBitsVar();
	auto frag_size = buf.readBitsVar();

	if (mFiles.count(path) == 0)
	{
		mFiles[path].buf.setSize(file_size);
	}

	auto& progress = mFiles[path].progress;
	auto& file_buf = mFiles[path].buf;

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

	STATS_INDICATE_GROUP("net", path, Common::Helpers::BytesToNiceString(progress) + "/" + Common::Helpers::BytesToNiceString(file_size));

	if (progress == file_size)
	{
		Platform::Asset::Write(path, file_buf.getMemory(), file_buf.getSize(), Platform::Asset::Storage::Bundle);
		mFiles.erase(path);
		LOG(path + " saved");
	}
}

// client

Client::Client() : Shared::NetworkingUDP::Client({ "192.168.0.106:1337" })
{
    //
}

std::shared_ptr<Shared::NetworkingUDP::Channel> Client::createChannel()
{
	return std::make_shared<hcg001::Channel>();
}

void Client::sendEvent(const std::string& name, const std::map<std::string, std::string>& params)
{
	if (!isConnected())
		return;

	getMyChannel()->sendEvent(name, params);
}
