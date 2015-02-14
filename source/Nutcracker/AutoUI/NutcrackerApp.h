/////////////////////////////////////////////////////////////////////////////
// Name:        NutcrackerApp.h
// Purpose:     
// Author:      Rui Figueira
// Modified by: 
// Created:     13/02/2015 21:02:05
// RCS-ID:      
// Copyright:   
// Licence:     
/////////////////////////////////////////////////////////////////////////////

#ifndef _NUTCRACKERAPP_H_
#define _NUTCRACKERAPP_H_


/*!
 * Includes
 */

////@begin includes
#include "wx/image.h"
////@end includes

/*!
 * Forward declarations
 */

////@begin forward declarations
////@end forward declarations

/*!
 * Control identifiers
 */

////@begin control identifiers
////@end control identifiers

/*!
 * NutcrackerApp class declaration
 */

class NutcrackerApp: public wxApp
{    
    DECLARE_CLASS( NutcrackerApp )
    DECLARE_EVENT_TABLE()

public:
    /// Constructor
    NutcrackerApp();

    void Init();

    /// Initialises the application
    virtual bool OnInit();

    /// Called on exit
    virtual int OnExit();

////@begin NutcrackerApp event handler declarations

////@end NutcrackerApp event handler declarations

////@begin NutcrackerApp member function declarations

////@end NutcrackerApp member function declarations

////@begin NutcrackerApp member variables
////@end NutcrackerApp member variables
};

/*!
 * Application instance declaration 
 */

////@begin declare app
DECLARE_APP(NutcrackerApp)
////@end declare app

#endif
    // _NUTCRACKERAPP_H_
