#pragma once

#include <shared/all.h>

class Guild
{
public:
	Guild(int index);

public:
	void save();
	void load();

public:
	auto getIndex() const { return mIndex; }

	const auto& getName() const { return mName; }
	void setName(const std::string& value) { mName = value; }

	const auto& getMembers() const { return mMembers; }
	void setMembers(const std::set<int>& value) { mMembers = value; }

private:
	int mIndex;
	std::string mName;
	std::set<int> mMembers;
};