#include "App.h"
#include "MainWindow.h"

#include <AboutWindow.h>
#include <Catalog.h>

#undef B_TRANSLATION_CONTEXT
#define B_TRANSLATION_CONTEXT "Application"

const char* kApplicationSignature = "application/x-vnd.EmirhanUcan-HydraDragonAntivirus";


App::App()
	:
	BApplication(kApplicationSignature)
{
	MainWindow* mainWindow = new MainWindow();
	mainWindow->Show();
}


App::~App()
{
}


void
App::AboutRequested()
{
	BAboutWindow* about
		= new BAboutWindow(B_TRANSLATE_SYSTEM_NAME("HydraDragonAntivirus"), kApplicationSignature);
	about->AddDescription(B_TRANSLATE("Detect and prevent ransomware detection"));
	about->AddCopyright(2024, "Emirhan Ucan");
	about->Show();
}


int
main()
{
	App* app = new App();
	app->Run();
	delete app;
	return 0;
}
