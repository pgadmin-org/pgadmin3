//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// Copyright (C) 2002, The pgAdmin Development Team
// This software is released under the pgAdmin Public Licence
//
// pgAdmin3.cpp - The application
//
//////////////////////////////////////////////////////////////////////////

// wxWindows headers
#include <wx/wx.h>
#include <wx/image.h>

// App headers
#include "pgAdmin3.h"
#include "ui/forms/frmMain.h"
#include "ui/forms/frmSplash.h"

IMPLEMENT_APP(pgAdmin3)

// Globals
frmMain *winMain;

// The Application!
bool pgAdmin3::OnInit()
{
  // We need JPEG Support
  wxImage::AddHandler(new wxJPEGHandler);
  
  // Show the splash screen
  frmSplash* winSplash = new frmSplash((wxFrame *)NULL);
  SetTopWindow(winSplash);
  winSplash->Show(TRUE);
  
#ifndef _DEBUG
  wxSleep(2);
#endif
  
  // Create & show the main form
  winMain = new frmMain("pgAdmin III", wxPoint(50, 50), wxSize(750, 550));
  winMain->Show(TRUE);
  
  winSplash->Close();
  
  return TRUE;
}
