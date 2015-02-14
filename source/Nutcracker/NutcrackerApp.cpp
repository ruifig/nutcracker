#include "NutcrackerPCH.h"
#include "NutcrackerApp.h"
#include "View/MainWnd.h"

using namespace cz::view;

// Create a new application object: this macro will allow wxWidgets to create
// the application object during program execution (it's better than using a
// static object for many reasons) and also implements the accessor function
// wxGetApp() which will return the reference of the right type (i.e. MyApp and
// not wxApp)
IMPLEMENT_APP(NutcrackerApp)

IMPLEMENT_CLASS(NutcrackerApp, wxApp)

BEGIN_EVENT_TABLE( NutcrackerApp, wxApp )
END_EVENT_TABLE()

NutcrackerApp::NutcrackerApp()
{
	Init();
}

void NutcrackerApp::Init()
{
}

bool NutcrackerApp::OnInit()
{
    // call the base class initialization method, currently it only parses a
    // few common command-line options but it could be do more in the future
    if ( !wxApp::OnInit() )
        return false;

 #if wxUSE_XPM
	wxImage::AddHandler(new wxXPMHandler);
#endif
#if wxUSE_LIBPNG
	wxImage::AddHandler(new wxPNGHandler);
#endif
#if wxUSE_LIBJPEG
	wxImage::AddHandler(new wxJPEGHandler);
#endif
#if wxUSE_GIF
	wxImage::AddHandler(new wxGIFHandler);
#endif

    // create the main application window
    MainWnd *mainWnd = new MainWnd();

    mainWnd->Show(true);

    // success: wxApp::OnRun() will be called which will enter the main message
    // loop and the application will run. If we returned false here, the
    // application would exit immediately.
    return true;
}

int NutcrackerApp::OnExit()
{
	return wxApp::OnExit();
}
