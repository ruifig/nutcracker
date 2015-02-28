#include "NutcrackerPCH.h"
#include "NutcrackerApp.h"
#include "MainWnd.h"
#include "AppEvents.h"
#include "UIDefs.h"
#include "Interpreter.h"
#include "Workspace.h"

#include "art/xpm/debugcursor.xpm"
namespace nutcracker
{

// Create a new application object: this macro will allow wxWidgets to create
// the application object during program execution (it's better than using a
// static object for many reasons) and also implements the accessor function
// wxGetApp() which will return the reference of the right type (i.e. MyApp and
// not wxApp)
IMPLEMENT_APP(NutcrackerApp)

IMPLEMENT_CLASS(NutcrackerApp, wxApp)

BEGIN_EVENT_TABLE( NutcrackerApp, wxApp )
	EVT_COMMAND(wxID_ANY, NUTCRACKER_LAMBDA_EVENT, NutcrackerApp::OnLambdaEvent)
END_EVENT_TABLE()

NutcrackerApp::NutcrackerApp()
{
	auto cfg = cz::PlatformRoot::Config{ nullptr, 0, false };
	m_platformRoot = std::make_unique<cz::PlatformRoot>(&cfg);
	Init();
}

void NutcrackerApp::Init()
{
}

bool NutcrackerApp::OnInit()
{
	gParameters = std::make_unique<cz::Parameters>();
	gParameters->set(argc, argv);

    // call the base class initialization method, currently it only parses a
    // few common command-line options but it could be do more in the future
	// NOTE: Not calling it, as it will show an error message because the I try to use my own command lines
    // if ( !wxApp::OnInit() )
    //   return false;


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


	//
	// Initialize image lists
	//
	for (int i = 0; i < SMALLIMG_IDX_MAX; i++)
		gImageListSmall.Add(wxArtProvider::GetBitmap(wxART_MISSING_IMAGE, wxART_OTHER, wxSize(16, 16)));
	gImageListSmall.Replace(SMALLIMG_IDX_FOLDER, wxArtProvider::GetBitmap(wxART_FOLDER, wxART_OTHER, wxSize(16, 16)));
	gImageListSmall.Replace(SMALLIMG_IDX_FILE_OTHER, wxArtProvider::GetBitmap(wxART_NORMAL_FILE, wxART_OTHER, wxSize(16, 16)));
	gImageListSmall.Replace(SMALLIMG_IDX_FILE_NUT, wxBITMAP_PNG(APP_IMG_16x16_FILE_NUT));
	gImageListSmall.Replace(SMALLIMG_IDX_REFRESH, wxBITMAP_PNG(APP_IMG_16x16_REFRESH));
	gImageListSmall.Replace(SMALLIMG_IDX_NUT, wxBITMAP_PNG(APP_IMG_16x16_NUT));
	gImageListSmall.Replace(SMALLIMG_IDX_CALLSTACK_CURRENT, wxBITMAP_PNG(APP_IMG_16x16_CALLSTACK_CURRENT));

	for (int i = 0; i < BIGIMG_IDX_NUT; i++)
		gImageList32x32.Add(wxArtProvider::GetBitmap(wxART_MISSING_IMAGE, wxART_OTHER, wxSize(32, 32)));
	gImageList32x32.Replace(BIGIMG_IDX_NUT, wxBITMAP_PNG(APP_IMG_32x32_NUT));

	gWorkspace = std::make_shared<Workspace>();
	// create the main application window
    MainWnd *mainWnd = new MainWnd();

    mainWnd->Show(true);

	if (gParameters->has("workspace"))
		gWorkspace->addFolder(gParameters->get("workspace"));

	/*
	postAppLambdaEvent([]()
	{
		fireAppEvent(AppEventID::OpenWorkspace);
	});
	*/

	std::shared_ptr<std::function<void()>> func;

	//
	// Test project handling
	//

    // success: wxApp::OnRun() will be called which will enter the main message
    // loop and the application will run. If we returned false here, the
    // application would exit immediately.
    return true;
}

int NutcrackerApp::OnExit()
{
	gWorkspace.reset();
	gUIState.reset();
	gParameters.reset();
	return wxApp::OnExit();
}

void NutcrackerApp::OnLambdaEvent(wxCommandEvent& evt)
{
	if (gShuttingDown)
		return;
	static_cast<NutcrackerLambdaEvent&>(evt).run();
}

} // namespace nutcracker
