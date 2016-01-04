//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// wxgridsel.h - replacement for wx/generic/gridsel.h
// if binary wxWindows 2.4.0 distribution is used.
// This file is necessary until wxGrid supports multiple wxGrid::SetTable() calls
//
//////////////////////////////////////////////////////////////////////////

#ifndef __WXGRIDSEL_H
#define __WXGRIDSEL_H


class wxGridSelection
{
	wxGridCellCoordsArray dummy1[3];
	wxArrayInt dummy2[2];
	void *dummy3;
	wxGrid::wxGridSelectionModes dummy4;
};

#endif
