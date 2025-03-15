#include "application.h"

void sky_main()
{
	sky::Locator<sky::Application>::Init(PROJECT_NAME,
		std::set{ sky::Application::Flag::Audio, sky::Application::Flag::Scene });
	{
		hcg001::Application game;
		sky::Locator<sky::Application>::Get()->run();
	}
	sky::Locator<sky::Application>::Reset();
}