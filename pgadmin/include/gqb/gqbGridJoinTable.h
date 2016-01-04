//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// gqbGridJoinTable.h - Table implementation for Join Panel Grid
//
//////////////////////////////////////////////////////////////////////////

#ifndef GQBGRIDJOINTABLE_H
#define GQBGRIDJOINTABLE_H

#include <wx/grid.h>
#include <wx/laywin.h>

// App headers
#include "gqb/gqbQueryObjs.h"
#include "gqb/gqbArrayCollection.h"

class gqbController;

class gqbGridJoinTable : public wxGridTableBase
{
public:
	gqbGridJoinTable(gqbController *_controller);
	~gqbGridJoinTable();
	int GetNumberRows();
	int GetNumberCols();
	bool IsEmptyCell(int row, int col);
	wxString GetValue(int row, int col);
	wxString GetColLabelValue( int col);
	void SetValue(int row, int col, const wxString &value);
	void AppendJoin(gqbQueryJoin *item);
	void removeJoin(gqbQueryJoin *item);
	void removeJoins(gqbQueryObject *obj);
	void emptyTableData();
	gqbQueryObject *DeleteRow(size_t pos);
	bool ReplaceJoin(gqbQueryJoin *orig, gqbQueryJoin *newVal);
	gqbQueryJoin *GetJoin(int row);
	void selectJoin(gqbQueryJoin *join);

private:
	gqbController *controller;
	gqbArrayCollection joins;
};

#endif

