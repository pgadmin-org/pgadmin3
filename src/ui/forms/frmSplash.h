//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// Copyright (C) 2002, The pgAdmin Development Team
// This software is released under the pgAdmin Public Licence
//
// frmSplash.h - Splash Screen
//
//////////////////////////////////////////////////////////////////////////

// Class declarations
class frmSplash : public wxFrame
{
public:
    frmSplash();
	void OnPaint(wxPaintEvent&);
	
private:
	wxBitmap imgSplash;
    DECLARE_EVENT_TABLE()
};

