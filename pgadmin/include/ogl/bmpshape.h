//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Portions Copyright (C) 1998 - 2011, Julian Smart
// Portions Copyright (C) 2011 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// bmpshape.h - wxBitmapShape
//
//////////////////////////////////////////////////////////////////////////

#ifndef _OGL_BITMAP_H_
#define _OGL_BITMAP_H_

#include "wx/bitmap.h"

class wxBitmapShape: public wxRectangleShape
{
	DECLARE_DYNAMIC_CLASS(wxBitmapShape)
public:
	wxBitmapShape();
	~wxBitmapShape();

	void OnDraw(wxDC &dc);

#if wxUSE_PROLOGIO
	// I/O
	void WriteAttributes(wxExpr *clause);
	void ReadAttributes(wxExpr *clause);
#endif

	// Does the copying for this object
	void Copy(wxShape &copy);

	void SetSize(double w, double h, bool recursive = TRUE);
	inline wxBitmap &GetBitmap() const
	{
		return (wxBitmap &) m_bitmap;
	}
	void SetBitmap(const wxBitmap &bm);
	inline void SetFilename(const wxString &f)
	{
		m_filename = f;
	};
	inline wxString GetFilename() const
	{
		return m_filename;
	}

private:
	wxBitmap      m_bitmap;
	wxString      m_filename;
};

#endif
// _OGL_BITMAP_H_


