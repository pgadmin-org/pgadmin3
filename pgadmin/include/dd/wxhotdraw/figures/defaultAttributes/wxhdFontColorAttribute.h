//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2011, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// wxhdFontAttribute.cpp - Default attribute for color of fonts
//
//////////////////////////////////////////////////////////////////////////

#ifndef WXHDFONTCOLORATTRIBUTE_H
#define WXHDFONTCOLORATTRIBUTE_H

#include "dd/wxhotdraw/figures/wxhdAttribute.h"

class wxhdFontColorAttribute : public wxhdAttribute
{
public:
	wxhdFontColorAttribute();
	virtual void callDefaultChangeDialog(wxWindow *owner = NULL);
	virtual void apply(wxBufferedDC &context);
//	void setFontColorAttribute(wxColour color);
	wxColour fontColor;
protected:


};
#endif
