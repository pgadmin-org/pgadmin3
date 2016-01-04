//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// frmAbout.h - About Box
//
//////////////////////////////////////////////////////////////////////////

#ifndef FRMABOUT_H
#define FRMABOUT_H

// Class declarations
class frmAbout : public wxFrame
{
public:
	frmAbout(wxFrame *parent);

	void OnPaint(wxPaintEvent &);

private:
	void SetWindowShape();
	void OnWindowCreate(wxWindowCreateEvent &WXUNUSED(evt));
	void OnLeftDown(wxMouseEvent &WXUNUSED(evt));
	void OnKeyUp(wxKeyEvent &evt);

	wxBitmap about;
	DECLARE_EVENT_TABLE()
};

class aboutFactory : public actionFactory
{
public:
	aboutFactory(menuFactoryList *list, wxMenu *mnu, ctlMenuToolbar *toolbar);
	wxWindow *StartDialog(frmMain *form, pgObject *obj);
};


#endif
