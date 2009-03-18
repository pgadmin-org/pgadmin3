//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2009, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// gqbTable.h - Table object for GQB
//
//////////////////////////////////////////////////////////////////////////

#ifndef GQBTABLE_H
#define GQBTABLE_H

// App headers
#include "gqb/gqbObject.h"
#include "gqb/gqbBrowser.h"
#include "gqb/gqbObjectCollection.h"

class gqbColumn;

// Create Array Objects used as base for gqbCollections
class gqbTable : public gqbObjectCollection
{
public:
    gqbTable(gqbObject *parent, wxString name, type_gqbObject type);
    void createObjects(gqbBrowser *_tablesBrowser,  pgConn *_conn, OID oidVal, wxTreeItemId parentNode);
    gqbIteratorBase* createColumnsIterator();
    int countCols();
    gqbColumn* getColumnAtIndex(int index);
    int indexColumn(gqbColumn *col);

private:
    wxString NumToStr(OID value);
    void addColumn(gqbColumn *column);    // Used only as synonym for gqbObjectCollection addObject
    void createColumns(pgConn *conn, gqbBrowser *tablesBrowser, wxTreeItemId parentNode,  OID oidVal);

};
#endif
