#include "ui/ApolloApp.h"
#include "ui/livepage.h"
#include "seeker/logger.h"

bool ApolloApp::OnInit() {
	seeker::Logger::init();
	auto* frame = new ApolloLivePage(nullptr);
	frame->Show(true);
	return true;
}
