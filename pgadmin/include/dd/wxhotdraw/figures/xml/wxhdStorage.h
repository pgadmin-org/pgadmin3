//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2011, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// wxhdAbstractFigure.h - Base class for all figures with attributes (line size, fonts and others)
//
//////////////////////////////////////////////////////////////////////////

#ifndef WXHDSTORAGE_H
#define WXHDSTORAGE_H

#include "dd/wxhotdraw/main/wxhdObject.h"
#include "dd/wxhotdraw/figures/wxhdIFigure.h"

class wxhdStorage : public wxhdObject
{
public:
	wxhdStorage();
	static bool Read(xmlTextReaderPtr reader);
	static bool Write(xmlTextWriterPtr writer, wxhdIFigure *figure);
};
#endif
