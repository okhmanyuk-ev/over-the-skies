#pragma once

#include <shared/all.h>

class Guild
{
public:
	static inline const int NoneGuild = -1;

public:
	Guild(int index);

public:
	void save();
	void load();

public:
	auto getIndex() const { return mIndex; }

	const auto& getName() const { return mName; }
	void setName(const std::string& value) { mName = value; }

	auto getScore() const { return mScore; }
	void setScore(int value) { mScore = value; }

	auto& getMembers() { return mMembers; }
	
private:
	int mIndex;
	std::string mName;
	int mScore = 0;
	std::set<int> mMembers;
};