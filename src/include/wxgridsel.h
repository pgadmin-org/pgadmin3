//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// Copyright (C) 2002, The pgAdmin Development Team
// This software is released under the pgAdmin Public Licence
//
// wxgridsel.h - replacement for wx/generic/gridsel.h
// if binary wxWindows 2.4.0 distribution is used.
// This file is necessary until wxGrid supports multiple wxGrid::SetTable() calls
//
//////////////////////////////////////////////////////////////////////////

class wxGridSelection
{
    wxGridCellCoordsArray dummy1[3];
    wxArrayInt dummy2[2];
    void *dummy3;
    wxGrid::wxGridSelectionModes dummy4;
};

