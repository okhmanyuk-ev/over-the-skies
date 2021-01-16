#pragma once

#include <shared/all.h>
#include "hud.h"
#include "client.h"

namespace hcg001::Helpers
{
	const float ButtonRounding = 0.33f;

	const auto BaseWindowColor = glm::rgbColor(glm::vec3(210.0f, 0.5f, 1.0f));

	inline std::shared_ptr<Hud> gHud = nullptr;

	struct PrintEvent
	{
		std::string text;
	};

	class Label : public Scene::Label
	{
	public:
		Label();
	};

	class TextInputField : public Scene::Clickable<Scene::Node>,
		public Common::Event::Listenable<Shared::TouchEmulator::Event>,
		public Common::Event::Listenable<Platform::System::VirtualKeyboardTextChanged>
	{
	public:
		TextInputField();

	private:
		void onEvent(const Shared::TouchEmulator::Event& e) override;
		void onEvent(const Platform::System::VirtualKeyboardTextChanged& e) override;

	public:
		auto getLabel() const { return mLabel; }

	private:
		std::shared_ptr<Label> mLabel;
	};
}