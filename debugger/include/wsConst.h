//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2007, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// wsConst.h - debugger 
//
//////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
//
// Constants and enumerator Identifiers for the entire workstation application
//
////////////////////////////////////////////////////////////////////////////////
#ifndef WSCONSTH
#define WSCONSTH

#define	INCLUDE_WATCHPOINTS	0	// Enable watchpoint support? (currently incomplete)
#define INCLUDE_FUNCTION_EDITOR	0	// Enable function editor infrastructure?

const int 	ID_BTNNEXT	   = 1800;
const int  	ID_GRDFUNCARGS = 1810;
const int  	ID_TXTMESSAGE  = 1820;
const int  	ID_TIMER	   = 1830;
const int  	ID_BTNCANCEL   = 1840;

const int  	ID_PARAMGRID   = 1000;
const int  	ID_VARGRID	   = 1001;
const int 	ID_MSG_PAGE	   = 1002;
const int	ID_PKGVARGRID  = 1003;
#endif

