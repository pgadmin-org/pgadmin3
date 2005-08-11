//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2005, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// misc.h - Miscellaneous Utilties
//
//////////////////////////////////////////////////////////////////////////

#ifndef MISC_H
#define MISC_H

#include "base/base.h"


// compile ID and Name into one string
wxString IdAndName(long id, const wxString &name);

// Quoting
wxString qtIdent(const wxString& value);    // add " if necessary
wxString qtTypeIdent(const wxString& value);    // add " if necessary
wxString qtStringDollar(const wxString &value);
wxString qtStrip(const wxString& value);    // remove \"


// string build helper
void AppendIfFilled(wxString &str, const wxString &delimiter, const wxString &what);

// Create keyword list from PostgreSQL list
void FillKeywords(wxString &str);

// Fill array, splitting the string separated by commas (maybe quoted elements)
void FillArray(wxArrayString &array, const wxString &str);


// splitting of strings, obeying quotes
class queryTokenizer : public wxStringTokenizer
{
public:
    queryTokenizer(const wxString& str, const wxChar delim=(wxChar)' ');
    wxString GetNextToken();
private:
    char delimiter;
};


// File handling including encoding according to sysSettings if format<0,
// 0-> local charset, 1->utf8
wxString FileRead(const wxString &filename, int format=-1);
bool FileWrite(const wxString &filename, const wxString &data, int format=-1);

void DisplayHelp(wxWindow *wnd, const wxString &helpTopic, char **icon=0);
void DisplaySqlHelp(wxWindow *wnd, const wxString &helpTopic, char **icon=0);

#ifndef WIN32
wxString ExecProcess(const wxString &cmd);
#endif



enum 
{
    PGM_SERVER,
    PGM_DATABASE,
    PGM_SCHEMA,
    PGM_UNKNOWN,
    PGM_FUNCTION,
    PGM_TABLE,
    PGM_VIEW,
    PGM_SEQUENCE,
};

enum        // depends on pgaFactory browserImages->Add order!
{
    PGICON_PROPERTY,
    PGICON_STATISTICS,
    PGICON_CHECK,
    PGICON_COLUMN,
    PGICON_INDEX,
    PGICON_RULE,
    PGICON_TRIGGER,
    PGICON_FOREIGNKEY,
    PGICON_CONSTRAINT,
    PGICON_PRIMARYKEY,
    PGICON_UNIQUE,
    PGICON_PUBLIC,

    PGAICON_JOB,
    PGAICON_JOBDISABLED,
    PGAICON_STEP,
    PGAICON_SCHEDULE,

    SLICON_NODE,
    SLICON_PATH,
    SLICON_LISTEN,
    SLICON_SET,
    SLICON_SET2,
    SLICON_SUBSCRIPTION,
    SLICON_SUBSCRIPTION2
};



#endif
