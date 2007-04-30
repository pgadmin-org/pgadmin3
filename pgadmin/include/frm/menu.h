//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2007, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// misc.h - Miscellaneous Utilties
//
//////////////////////////////////////////////////////////////////////////


#ifndef __MENU_H
#define __MENU_H

// Menu options
enum
{
    MNU_ADDSERVER = 101,
    MNU_SAVEDEFINITION,
    MNU_EXIT,
    MNU_CONTEXTMENU,
    MNU_SYSTEMOBJECTS,
    MNU_SQLPANE,
    MNU_OBJECTBROWSER,
    MNU_TOOLBAR,
    MNU_LIMITBAR,
    MNU_DATABASEBAR,
    MNU_SCRATCHPAD,
    MNU_OUTPUTPANE,
    MNU_DEFAULTVIEW,
    MNU_BACK,
    MNU_FORWARD,
    MNU_REFRESH,

    MNU_ADDCOLUMN,
	MNU_CLOSE,
	MNU_MIN,
    MNU_RECORD,
    MNU_STOP,
    MNU_APPEND,
    MNU_DELETE,
    MNU_OPEN,
    MNU_SAVE,
    MNU_SAVEAS,
    MNU_EXPORT,
    MNU_OPTIONS,
    MNU_CUT,
    MNU_COPY,
    MNU_PASTE,
    MNU_CLEAR,
    MNU_FIND,
    MNU_REPLACE,
    MNU_UNDO,
    MNU_REDO,
    MNU_CANCEL,
    MNU_EXECUTE,
    MNU_EXECFILE,
    MNU_EXPLAIN,
    MNU_EXPLAINOPTIONS,
    MNU_EXPLAINTEXT,
    MNU_VERBOSE,
    MNU_ANALYZE,
    MNU_CLEARHISTORY,
    MNU_SAVEHISTORY,
    MNU_CHECKALIVE,
	MNU_SELECTALL,

    MNU_CONTENTS,
    MNU_HELP,
    MNU_HINT,

    MNU_CONFIGSUBMENU,
    MNU_SLONY_SUBMENU,

    MNU_ONLINEUPDATE_NEWDATA,

	MNU_AUTOCOMPLETE,
    MNU_WORDWRAP,
	MNU_SHOWWHITESPACE,
	MNU_SHOWLINEENDS,
    MNU_QUICKREPORT,

    MNU_CR,
    MNU_CRLF,
    MNU_LF,

    MNU_RECENT,
    MNU_NEW=MNU_RECENT+15,  // leave space for recent file entries

    MNU_ACTION=MNU_NEW+1000, // leave space for objects

	MNU_FAVOURITES_ADD=MNU_ACTION+1000, // leave space for actions
	MNU_FAVOURITES_MANAGE

};

#endif
