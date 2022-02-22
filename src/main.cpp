#include "ui/ApolloApp.h"

#ifdef _WIN32

#if NOT_USE_CONSOLE

#pragma comment( linker, "/subsystem:\"windows\" /entry:\"mainCRTStartup\"" )

#endif //NOT_USE_CONSOLE

//#pragma comment( linker, "/subsystem:\"windows\" /entry:\"mainCRTStartup\"" )

int main() {
	ApolloApp* app = new ApolloApp();
	wxApp::SetInstance(app);
	return wxEntry();
}
//	IMPLEMENT_APP_CONSOLE(TheiaApp);


#else

IMPLEMENT_APP(KankanApp);

#endif