#pragma once

#include <shared/all.h>

namespace hcg001
{
	class Guild
	{
	public:
		auto getJson() const { return mJson; }
		void setJson(nlohmann::json value) { mJson = value; }

	private:
		nlohmann::json mJson;
	};
}
