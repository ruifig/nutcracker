#include "NutcrackerPCH.h"
#include "NutcrackerApp.h"
#include "View/MainWnd.h"
#include "Document/Project.h"
#include "AppEvents.h"
#include "UIDefs.h"

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

NutcrackerApp::NutcrackerApp() : m_platformRoot(&cz::PlatformRoot::Config{nullptr, 0, false})
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


	//
	// Initialize image lists
	//
	for (int i = 0; i < SMALLIMG_IDX_MAX; i++)
		gImageListSmall.Add(wxArtProvider::GetBitmap(wxART_MISSING_IMAGE, wxART_OTHER, wxSize(16, 16)));
	gImageListSmall.Replace(SMALLIMG_IDX_FOLDER, wxArtProvider::GetBitmap(wxART_FOLDER, wxART_OTHER, wxSize(16, 16)));
	gImageListSmall.Replace(SMALLIMG_IDX_FILE_OTHER, wxArtProvider::GetBitmap(wxART_NORMAL_FILE, wxART_OTHER, wxSize(16, 16)));
	gImageListSmall.Replace(SMALLIMG_IDX_FILE_NUT, wxBITMAP_PNG(APP_IMG_16x16_FILE_NUT));
	gImageListSmall.Replace(SMALLIMG_IDX_NUT, wxBITMAP_PNG(APP_IMG_16x16_NUT));

	for (int i = 0; i < BIGIMG_IDX_NUT; i++)
		gImageList32x32.Add(wxArtProvider::GetBitmap(wxART_MISSING_IMAGE, wxART_OTHER, wxSize(32, 32)));
	gImageList32x32.Replace(BIGIMG_IDX_NUT, wxBITMAP_PNG(APP_IMG_32x32_NUT));

	// create the main application window
    MainWnd *mainWnd = new MainWnd();

    mainWnd->Show(true);

	using namespace cz::document;
	using namespace cz::view;
	gProject = std::make_shared<Project>(cz::Filesystem::getSingleton().getCWD().c_str());
	gProject->populate();
	mainWnd->addAsyncFunc([]()
	{
		fireAppEvent(AppEventID::OpenWorkspace);
	});


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
	gProject.reset();
	return wxApp::OnExit();
}
