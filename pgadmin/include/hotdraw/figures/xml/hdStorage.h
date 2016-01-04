//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// hdAbstractFigure.h - Base class for all figures with attributes (line size, fonts and others)
//
//////////////////////////////////////////////////////////////////////////

#ifndef HDSTORAGE_H
#define HDSTORAGE_H

#include "hotdraw/main/hdObject.h"
#include "hotdraw/figures/hdIFigure.h"

class hdStorage : public hdObject
{
public:
	hdStorage();
	static bool Read(xmlTextReaderPtr reader);
	static bool Write(xmlTextWriterPtr writer, hdIFigure *figure);
};
#endif
