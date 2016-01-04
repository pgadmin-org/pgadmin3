//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// hdFontAttribute.h - Default attribute for attributes of fonts
//
//////////////////////////////////////////////////////////////////////////

#ifndef HDFONTATTRIBUTE_H
#define HDFONTATTRIBUTE_H

#include "hotdraw/figures/hdAttribute.h"

class hdFontAttribute : public hdAttribute
{
public:
	hdFontAttribute();
	virtual void callDefaultChangeDialog(wxWindow *owner = NULL);
	virtual void apply(wxBufferedDC &context);
	wxFont &font();
	static wxFont *defaultFont;
	static void InitFont();
protected:
	wxFont fontAttributes;
};
#endif
