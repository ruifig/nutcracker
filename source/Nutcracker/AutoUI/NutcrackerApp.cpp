/////////////////////////////////////////////////////////////////////////////
// Name:        NutcrackerApp.cpp
// Purpose:     
// Author:      Rui Figueira
// Modified by: 
// Created:     13/02/2015 21:02:05
// RCS-ID:      
// Copyright:   
// Licence:     
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

////@begin includes
////@end includes

#include "NutcrackerApp.h"

////@begin XPM images
////@end XPM images


/*
 * Application instance implementation
 */

////@begin implement app
IMPLEMENT_APP( NutcrackerApp )
////@end implement app


/*
 * NutcrackerApp type definition
 */

IMPLEMENT_CLASS( NutcrackerApp, wxApp )


/*
 * NutcrackerApp event table definition
 */

BEGIN_EVENT_TABLE( NutcrackerApp, wxApp )

////@begin NutcrackerApp event table entries
////@end NutcrackerApp event table entries

END_EVENT_TABLE()


/*
 * Constructor for NutcrackerApp
 */

NutcrackerApp::NutcrackerApp()
{
    Init();
}


/*
 * Member initialisation
 */

void NutcrackerApp::Init()
{
////@begin NutcrackerApp member initialisation
////@end NutcrackerApp member initialisation
}

/*
 * Initialisation for NutcrackerApp
 */

bool NutcrackerApp::OnInit()
{    
////@begin NutcrackerApp initialisation
	// Remove the comment markers above and below this block
	// to make permanent changes to the code.

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
////@end NutcrackerApp initialisation

    return true;
}


/*
 * Cleanup for NutcrackerApp
 */

int NutcrackerApp::OnExit()
{    
////@begin NutcrackerApp cleanup
	return wxApp::OnExit();
////@end NutcrackerApp cleanup
}

