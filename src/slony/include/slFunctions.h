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


class frmMain;
class dlgProperty;

class slFunctions
{
public:
    static bool MergeSet(frmMain *frame, pgObject *obj);
    static bool MoveSet(frmMain *frame, pgObject *obj);
    static bool Failover(frmMain *frame, pgObject *obj);
    static bool UpgradeNode(frmMain *frame, pgObject *obj);
    static bool RestartNode(wxFrame *frame, pgObject *obj);

private:
    static bool Show(dlgProperty *dlg, frmMain *frame, pgObject *obj);
};
