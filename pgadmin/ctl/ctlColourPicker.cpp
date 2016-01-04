//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the BSD Licence
//
// ctlColourPicker.cpp - Colour Picker with a wxBitmapButton
//
//////////////////////////////////////////////////////////////////////////

// wxWindows headers
#include <wx/wx.h>
#include <wx/colour.h>
#include <wx/colordlg.h>

// App headers
#include "pgAdmin3.h"
#include "ctl/ctlColourPicker.h"


void ctlColourPicker::Create(wxWindow *parent, wxWindowID id, const wxPoint &pos, const wxSize &size)
{
	// Set Default Title
	m_title = _("Choose the colour");

	// Create the wxBitmapButton
	((wxBitmapButton *)this)->Create(parent, id, wxNullBitmap, pos, size);

	// Set the handler for a click
	Connect(id, wxEVT_LEFT_DOWN, wxMouseEventHandler(ctlColourPicker::DoProcessLeftClick) );
}


void ctlColourPicker::DoProcessLeftClick(wxMouseEvent &event)
{
	wxColourData clrData;

	// Initialise custom colours
	for (int index = 0; index < 16; index++)
		clrData.SetCustomColour(index, settings->GetCustomColour(index));

	// If there is an initial colour, set it for wxColourDialog
	if (m_colour_clr.IsOk())
		clrData.SetColour(m_colour_clr);

	// Declare the new dialog
	wxColourDialog dialog(this, &clrData);

	// and set its title
	dialog.SetTitle(m_title);

	// Now, show it
	if (dialog.ShowModal() == wxID_OK)
	{
		clrData = dialog.GetColourData();
		SetColour(clrData.GetColour());

		// Store custom colours
		for (int index = 0; index < 16; index++)
			settings->SetCustomColour(index, clrData.GetCustomColour(index).GetAsString(wxC2S_HTML_SYNTAX));
	}
}


void ctlColourPicker::UpdateColour()
{
	if (!m_colour_clr.IsOk())
	{
		wxLogError(wxT("ohoh"));
		wxBitmap empty(1, 1);
		SetBitmapLabel(empty);
		return;
	}

	wxSize sz = GetSize();
	sz.x -= 2 * GetMarginX();
	sz.y -= 2 * GetMarginY();

	wxPoint topleft;
	if ( sz.x < 1 )
		sz.x = 1;
	else if ( sz.y < 1 )
		sz.y = 1;
	wxBitmap bmp(sz.x, sz.y);

	wxMemoryDC dc(bmp);
	dc.SetBrush(wxBrush(m_colour_clr));
	dc.DrawRectangle(topleft, sz);

	((wxBitmapButton *)this)->SetBitmapLabel(bmp);
}

wxColour ctlColourPicker::GetColour()
{
	return m_colour_clr;
}

wxString ctlColourPicker::GetColourString()
{
	if (!m_colour_clr.IsOk())
		return wxEmptyString;
	return m_colour_clr.GetAsString();
}

void ctlColourPicker::SetColour(const wxColour &colour)
{
	m_colour_clr = colour;
	UpdateColour();
}

void ctlColourPicker::SetColour(const wxString &colour)
{
	m_colour_clr = wxColour(colour);
	UpdateColour();
}

void ctlColourPicker::SetTitle(const wxString &title)
{
	m_title = title;
}
