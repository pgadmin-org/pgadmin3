//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// Copyright (C) 2002, The pgAdmin Development Team
// This software is released under the pgAdmin Public Licence
//
// frmAbout.h - About Box
//
//////////////////////////////////////////////////////////////////////////

// Class declarations
class frmAbout : public wxDialog
{
public:
  frmAbout(wxFrame *parent);
  void OnPaint(wxPaintEvent&);
  
private:
  wxBitmap imgAbout;
  DECLARE_EVENT_TABLE()
};

