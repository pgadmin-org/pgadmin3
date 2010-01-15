//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2010, The pgAdmin Development Team
// This software is released under the BSD Licence
//
// gqbColumn.cpp - Column Object for GQB
//
//////////////////////////////////////////////////////////////////////////

// App headers
#include "pgAdmin3.h"

// wxWindows headers
#include <wx/wx.h>

// App headers
#include "gqb/gqbColumn.h"
#include "gqb/gqbObject.h"
#include "gqb/gqbSchema.h"
#include "gqb/gqbTable.h"
#include "gqb/gqbArrayCollection.h"

gqbColumn::gqbColumn(gqbObject *parent, wxString name, pgConn *connection):
gqbObject(name, parent, connection)
{
    setType(GQB_COLUMN);
}
