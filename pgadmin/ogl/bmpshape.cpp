//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Portions Copyright (C) 1998 - 2011, Julian Smart
// Portions Copyright (C) 2011 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// bmpshape.cpp - Bitmap shape class
//
//////////////////////////////////////////////////////////////////////////

#include "pgAdmin3.h"

#include "ogl/ogl.h"

/*
 * Bitmap object
 *
 */

IMPLEMENT_DYNAMIC_CLASS(wxBitmapShape, wxRectangleShape)

wxBitmapShape::wxBitmapShape(): wxRectangleShape(100.0, 50.0)
{
	m_filename = wxEmptyString;
}

wxBitmapShape::~wxBitmapShape()
{
}

void wxBitmapShape::OnDraw(wxDC &dc)
{
	if (!m_bitmap.Ok())
		return;

	double x, y;
	x = WXROUND(m_xpos - m_bitmap.GetWidth() / 2.0);
	y = WXROUND(m_ypos - m_bitmap.GetHeight() / 2.0);
	dc.DrawBitmap(m_bitmap, (int) x, (int) y, true);
}

void wxBitmapShape::SetSize(double w, double h, bool WXUNUSED(recursive))
{
	if (m_bitmap.Ok())
	{
		w = m_bitmap.GetWidth();
		h = m_bitmap.GetHeight();
	}

	SetAttachmentSize(w, h);

	m_width = w;
	m_height = h;
	SetDefaultRegionSize();
}

#if wxUSE_PROLOGIO
void wxBitmapShape::WriteAttributes(wxExpr *clause)
{
	// Can't really save the bitmap; so instantiate the bitmap
	// at a higher level in the application, from a symbol library.
	wxRectangleShape::WriteAttributes(clause);
	clause->AddAttributeValueString(wxT("filename"), m_filename);
}

void wxBitmapShape::ReadAttributes(wxExpr *clause)
{
	wxRectangleShape::ReadAttributes(clause);
	clause->GetAttributeValue(wxT("filename"), m_filename);
}
#endif

// Does the copying for this object
void wxBitmapShape::Copy(wxShape &copy)
{
	wxRectangleShape::Copy(copy);

	wxASSERT( copy.IsKindOf(CLASSINFO(wxBitmapShape)) ) ;

	wxBitmapShape &bitmapCopy = (wxBitmapShape &) copy;

	bitmapCopy.m_bitmap = m_bitmap;
	bitmapCopy.SetFilename(m_filename);
}

void wxBitmapShape::SetBitmap(const wxBitmap &bm)
{
	m_bitmap = bm;
	if (m_bitmap.Ok())
		SetSize(m_bitmap.GetWidth(), m_bitmap.GetHeight());
}


