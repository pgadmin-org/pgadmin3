//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// Copyright (C) 2002 - 2003, The pgAdmin Development Team
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
    MNU_PASSWORD,
    MNU_SAVEDEFINITION,
    MNU_EXIT,
    MNU_UPGRADEWIZARD,
    MNU_CONTEXTMENU,
    MNU_OPTIONS,
    MNU_SYSTEMOBJECTS,
    MNU_CONTENTS,
    MNU_PGSQLHELP,
    MNU_HELP,
    MNU_FAQ,
    MNU_BUGREPORT,
    MNU_TIPOFTHEDAY,
    MNU_ABOUT,
    MNU_BACK,
    MNU_FORWARD,
    MNU_REFRESH,
    MNU_CONNECT,
    MNU_DISCONNECT,
    MNU_NEWOBJECT,
    MNU_DROP,
    MNU_PROPERTIES,
    MNU_QUERYBUILDER,
    MNU_ADDTABLEVIEW,
	MNU_ADDCOLUMN,
	MNU_CLOSE,
	MNU_MIN,
    MNU_SQL,
    MNU_VIEWDATA,
    MNU_MAINTENANCE,
    MNU_CREATE,
    MNU_RECORD,
    MNU_STOP,
    MNU_STATUS,
    MNU_APPEND,
    MNU_DELETE,
    MNU_OPEN,
    MNU_SAVE,
    MNU_SAVEAS,
    MNU_EXPORT,
    MNU_CUT,
    MNU_COPY,
    MNU_PASTE,
    MNU_CLEAR,
    MNU_FIND,
    MNU_UNDO,
    MNU_REDO,
    MNU_CANCEL,
    MNU_EXECUTE,
    MNU_EXPLAIN,
    MNU_EXPLAINOPTIONS,
    MNU_VERBOSE,
    MNU_ANALYZE,
    MNU_RELOAD,
    MNU_CLEARHISTORY,
    MNU_SAVEHISTORY,
    MNU_RECENT, // leave space for recent file entries
    MNU_NEW=MNU_RECENT+15
};



#endif
