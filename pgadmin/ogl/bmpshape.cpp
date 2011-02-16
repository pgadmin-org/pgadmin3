/////////////////////////////////////////////////////////////////////////////
// Name:        bmpshape.cpp
// Purpose:     Bitmap shape class
// Author:      Julian Smart
// Modified by:
// Created:     12/07/98
// RCS-ID:      $Id: bmpshape.cpp,v 1.3 2007/01/16 02:36:41 anthemion Exp $
// Copyright:   (c) Julian Smart
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "ogl/oglprec.h"

#include "ogl/ogl.h"

/*
 * Bitmap object
 *
 */

IMPLEMENT_DYNAMIC_CLASS(wxBitmapShape, wxRectangleShape)

wxBitmapShape::wxBitmapShape():wxRectangleShape(100.0, 50.0)
{
  m_filename = wxEmptyString;
}

wxBitmapShape::~wxBitmapShape()
{
}

void wxBitmapShape::OnDraw(wxDC& dc)
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
  clause->AddAttributeValueString(_T("filename"), m_filename);
}

void wxBitmapShape::ReadAttributes(wxExpr *clause)
{
  wxRectangleShape::ReadAttributes(clause);
  clause->GetAttributeValue(_T("filename"), m_filename);
}
#endif

// Does the copying for this object
void wxBitmapShape::Copy(wxShape& copy)
{
  wxRectangleShape::Copy(copy);

  wxASSERT( copy.IsKindOf(CLASSINFO(wxBitmapShape)) ) ;

  wxBitmapShape& bitmapCopy = (wxBitmapShape&) copy;

  bitmapCopy.m_bitmap = m_bitmap;
  bitmapCopy.SetFilename(m_filename);
}

void wxBitmapShape::SetBitmap(const wxBitmap& bm)
{
  m_bitmap = bm;
  if (m_bitmap.Ok())
    SetSize(m_bitmap.GetWidth(), m_bitmap.GetHeight());
}

