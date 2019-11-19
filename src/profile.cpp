#include "profile.h"

using namespace hcg001;

void Profile::load()
{
	auto path = PLATFORM->getAppFolder() + "save.bson";

	if (!Platform::Asset::Exists(path, Platform::Asset::Path::Absolute))
		return;

	auto json_file = Platform::Asset(path, Platform::Asset::Path::Absolute);
	auto json = nlohmann::json::from_bson(std::string((char*)json_file.getMemory(), json_file.getSize()));

	if (json.contains("highscore"))
		mHighScore = json["highscore"];

	if (json.contains("rubies"))
		mRubies = json["rubies"];

	if (json.contains("skins"))
		mSkins = json["skins"].get<std::set<int>>();
}

void Profile::save()
{
	auto json = nlohmann::json();
	json["highscore"] = mHighScore;
	json["rubies"] = mRubies;
	json["skins"] = mSkins;
	auto bson = nlohmann::json::to_bson(json);
	Platform::Asset::Write(PLATFORM->getAppFolder() + "save.bson", bson.data(), bson.size(), Platform::Asset::Path::Absolute);
}

void Profile::clear()
{
	mHighScore = 0;
	mRubies = 0;
	mSkins = { 0 };
}