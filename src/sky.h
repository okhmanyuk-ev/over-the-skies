#pragma once

#include <Shared/all.h>

namespace hcg001
{
	class Sky : public Scene::Actionable<Scene::Node>
	{
	public:
		Sky();

	public:
		void changeColor(glm::vec3 top, glm::vec3 bottom);

	protected:
		void draw() override;
	
	private:
		std::shared_ptr<Scene::Color> mTopColor = std::make_shared<Scene::Color>();
		std::shared_ptr<Scene::Color> mBottomColor = std::make_shared<Scene::Color>();
	};
}