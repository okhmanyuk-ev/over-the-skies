#include "screen.h"

using namespace hcg001;

Screen::Screen()
{
	mGui = std::make_shared<Shared::SceneHelpers::SafeArea>();
	attach(mGui);
}
