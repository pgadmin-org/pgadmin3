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


#define HINT_CONNECTSERVER  wxT("conn-listen")
#define HINT_MISSINGHBA     wxT("conn-hba")
#define HINT_MISSINGIDENT   wxT("conn-ident")
#define HINT_PRIMARYKEY     wxT("pk")
#define HINT_FKINDEX        wxT("fki")

class frmMain;
class frmHint : public DialogWithHelp
{
public:
    static int ShowHint(wxWindow *fr,  const wxString &hint, const wxString &info=wxEmptyString);
    static bool WantHint(const wxString &hint);
    static void ResetHints();

private:
    frmHint(wxWindow *fr, int hint, const wxString &info=wxEmptyString);
    ~frmHint();

    static int GetHintNo(const wxString &hint);
    static bool WantHint(int hintno);
    wxString GetHelpPage() const;

    DECLARE_EVENT_TABLE();

    int currentHint;
};



#endif
