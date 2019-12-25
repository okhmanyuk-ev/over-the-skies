#pragma once

#include <Shared/all.h>

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

	public:
		std::shared_ptr<Scene::Sprite> mRuby = nullptr;
	};
}