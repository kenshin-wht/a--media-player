#include"ui/RenderTimer.h"

RenderTimer::RenderTimer(wxPanel *page) {
	panel = page;
}


void RenderTimer::Notify() {
	isTimeRender = true;
	panel->Refresh(false);
}

void RenderTimer::start() {
	wxTimer::Start(50);
}