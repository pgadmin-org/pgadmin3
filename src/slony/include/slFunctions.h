//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2005, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// slFunctions.h PostgreSQL Slony-I Functions
//
//////////////////////////////////////////////////////////////////////////


class slFunctions
{
public:
    static bool MergeSet(wxFrame *frame, pgObject *obj);
    static bool MoveSet(wxFrame *frame, pgObject *obj);
    static bool Failover(wxFrame *frame, pgObject *obj);
    static bool UpgradeNode(wxFrame *frame, pgObject *obj);
    static bool RestartNode(wxFrame *frame, pgObject *obj);
};