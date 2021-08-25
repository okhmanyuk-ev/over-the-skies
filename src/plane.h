#pragma once

#include <shared/all.h>

namespace hcg001
{	
	class Plane : public Scene::Rectangle
	{
	public:
		Plane();

	public:
		bool hasRubies() const { return !mRubies.empty(); }
		const auto& getRubies() const { return mRubies; }
		void addRuby(std::shared_ptr<Scene::Sprite> value) { mRubies.push_back(value); }

		bool isCrashed() const { return mCrashed; }
		void setCrashed(bool value) { mCrashed = value; }

		bool isPowerjump() const { return mPowerjump; }
		void setPowerjump(bool value) { mPowerjump = value; }

		bool isMoving() const { return mMoving; }
		void setMoving(bool value) { mMoving = value; }

	private:
		std::vector<std::shared_ptr<Scene::Sprite>> mRubies;
		bool mPowerjump = false;
		bool mMoving = false;
		bool mCrashed = false;
	};
}