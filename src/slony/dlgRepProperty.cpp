//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id: dlgRepProperty.cpp 4875 2006-01-06 21:06:46Z dpage $
// Copyright (C) 2002 - 2006, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// dlgRepProperty.cpp - PostgreSQL Property
//
//////////////////////////////////////////////////////////////////////////

// wxWindows headers
#include <wx/wx.h>

// App headers
#include "pgAdmin3.h"
#include "dlgRepProperty.h"
#include "slCluster.h"


dlgRepProperty::dlgRepProperty(frmMain *frame, slCluster *c, const wxString &resName)
: dlgProperty(frame, resName) 
{
    cluster=c;
}
