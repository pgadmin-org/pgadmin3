//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2004, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// frmHint.h - PostgreSQL Guru hints
//
//////////////////////////////////////////////////////////////////////////

#ifndef __FRMHINT
#define __FRMHINT



// must match frmHint.cpp hintArray!
typedef enum
{
    HintPrimaryKey  = 0,
    HintCoveringIndex,
    HintLast
} Hint;

class frmMain;
class frmHint : public DialogWithHelp
{
public:
    static int ShowHint(wxWindow *fr, Hint hintno);

private:
    frmHint(wxWindow *fr, Hint hintHo);
    ~frmHint();

    wxString GetHelpPage() const;

    DECLARE_EVENT_TABLE();
    Hint currentHint;
};



#endif
