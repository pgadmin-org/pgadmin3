//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// Copyright (C) 2002 - 2003, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// copyright.h - pgAdmin copyright messages
//
//////////////////////////////////////////////////////////////////////////


#define VERSION_WITH_DATE       wxT("Version ") VERSION wxT(" (") __TDATE__ wxT(")")
#define VERSION_WITHOUT_DATE    wxT("Version ") VERSION
#define COPYRIGHT               wxT("Copyright 2002 - 2003 The pgAdmin Development Team")
#define LICENSE                 _("This software is released under the Artistic License.")


#ifdef __WIN32__
#define SPLASH_FONTSIZE 8
#else
#if wxCHECK_VERSION(2,5,0)
#define SPLASH_FONTSIZE 9
#else
#define SPLASH_FONTSIZE 11
#endif
#endif

#define SPLASH_X0       6
#define SPLASH_Y0       114
#define SPLASH_OFFS     15
