#include "guild.h"

Guild::Guild(int index) : mIndex(index)
{
	//
}

void Guild::save()
{
	auto json = nlohmann::json();
	
	json["name"] = mName;
	json["members"] = mMembers;

	auto dump = json.dump(1);
	Platform::Asset::Write("guilds/" + std::to_string(mIndex) + ".json", dump.data(), dump.size(), Platform::Asset::Storage::Absolute);
}

void Guild::load()
{
	auto path = "guilds/" + std::to_string(mIndex) + ".json";

	assert(Platform::Asset::Exists(path, Platform::Asset::Storage::Absolute));
	
	auto json_file = Platform::Asset(path, Platform::Asset::Storage::Absolute);
	auto json_string = std::string((char*)json_file.getMemory(), json_file.getSize());
	auto json = nlohmann::json::parse(json_string);
	
	mName = json["name"];
	mMembers = json["members"].get<std::set<int>>();;
}
