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
    MNU_OPTIONS,
    MNU_SYSTEMOBJECTS,
    MNU_CONTENTS,
    MNU_PGSQLHELP,
    MNU_HELP,
    MNU_FAQ,
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
    MNU_SQL,
    MNU_VIEWDATA,
    MNU_VACUUM,
    MNU_CREATE,
    MNU_RECORD,
    MNU_STOP,
    MNU_STATUS,
    MNU_APPEND,
    MNU_DELETE,
    MNU_OPEN,
    MNU_SAVE,
    MNU_SAVEAS,
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
    MNU_RELOAD,
    MNU_RECENT, // leave space for recent file entries
    MNU_NEW=MNU_RECENT+15
};



#endif