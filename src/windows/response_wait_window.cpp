#include "response_wait_window.h"

using namespace hcg001;

ResponseWaitWindow::ResponseWaitWindow() : StandardWindow(false)
{
	setCloseOnMissclick(false);
	getBackground()->setSize({ 314.0f, 128.0f });
	getTitle()->setText(LOCALIZE("RESPONSE_WAIT_WINDOW_TITLE"));
}
