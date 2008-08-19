//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id: frmQuery.cpp 7381 2008-08-11 11:33:42Z dpage $
// Copyright (C) 2002 - 2008, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// gqbColumn.h - Column Object for GQB
//
//////////////////////////////////////////////////////////////////////////

#ifndef GQBCOLUMN_H
#define GQBCOLUMN_H

// App headers
#include "gqb/gqbObject.h"
#include "gqb/gqbTable.h"

// Create Array Objects used as base for gqbCollections
class gqbColumn : public gqbObject
{
public:
    gqbColumn(gqbObject *parent, wxString name, type_gqbObject type);
};
#endif
