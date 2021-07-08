#include "response_wait_window.h"

using namespace hcg001;

ResponseWaitWindow::ResponseWaitWindow() : Window(false)
{
	setCloseOnMissclick(false);
	getBackground()->setSize({ 314.0f, 128.0f });
	getTitle()->setText(LOCALIZE("RESPONSE_WAIT_WINDOW_TITLE"));

	auto wait_indicator = std::make_shared<Helpers::WaitingIndicator>();
	wait_indicator->setAnchor(0.5f);
	wait_indicator->setPivot(0.5f);
	getBody()->attach(wait_indicator);
}
