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
    HintConnectServer = 0,
    HintMissingHba,
    HintPrimaryKey,
    HintCoveringIndex,
    HintLast
} Hint;

class frmMain;
class wxHtmlWindow;
class frmHint : public DialogWithHelp
{
public:
    static int ShowHint(wxWindow *fr, Hint hintno, const wxString &info=wxEmptyString);
    static bool WantHint(Hint hintno);

private:
    frmHint(wxWindow *fr, Hint hintHo, const wxString &info=wxEmptyString);
    ~frmHint();

    wxString GetHelpPage() const;

    DECLARE_EVENT_TABLE();

    Hint currentHint;
};



#endif
