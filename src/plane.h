#pragma once

#include <shared/all.h>

namespace hcg001
{	
	class Plane : public Scene::Actionable<Scene::Node>, public Scene::Color
	{
	public:
		Plane();

	public:
		bool hasRuby() const { return mRuby != nullptr; }
		auto getRuby() const { return mRuby; }
		void setRuby(std::shared_ptr<Scene::Sprite> value) { mRuby = value; }

		bool isCrashed() const { return mCrashed; }
		void setCrashed(bool value) { mCrashed = value; }

		bool isPowerjump() const { return mPowerjump; }
		void setPowerjump(bool value) { mPowerjump = value; }

	private:
		std::shared_ptr<Scene::Sprite> mRuby = nullptr;
		bool mPowerjump = false;
		bool mCrashed = false;
	};
}