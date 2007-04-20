//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id: wsBreakPoint.cpp 6082 2007-03-16 12:58:18Z dpage $
// Copyright (C) 2002 - 2007, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// wsbreakpoint.cpp - debugger 
//
//////////////////////////////////////////////////////////////////////////

#include "pgAdmin3.h"

// wxWindows headers
#include <wx/wx.h>
#include <wx/listimpl.cpp>

// App headers
#include "debugger/wsBreakPoint.h"

WX_DEFINE_LIST( wsBreakpointList );


