//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// Copyright (C) 2002, The pgAdmin Development Team
// This software is released under the pgAdmin Public Licence
//
// copyright.h - pgAdmin copyright messages
//
//////////////////////////////////////////////////////////////////////////


#define VERSION_WITH_DATE       wxT("Version ") VERSION wxT(" (") __DATE__ wxT(")")
#define VERSION_WITHOUT_DATE    wxT("Version ") VERSION
#define COPYRIGHT               wxT("Copyright 2002, 2003 The pgAdmin Development Team")
#define LICENSE                 wxT("This software is released under the pgAdmin Public License.")


#ifdef __WIN32__
#define SPLASH_FONTSIZE 8
#else
#define SPLASH_FONTSIZE 11
#endif
#define SPLASH_X0       10
#define SPLASH_Y0       100
#define SPLASH_OFFS     15
