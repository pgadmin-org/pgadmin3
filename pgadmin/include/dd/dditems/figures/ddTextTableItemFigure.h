//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// Copyright (C) 2002 - 2011, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// gqbObject.h -
//
//////////////////////////////////////////////////////////////////////////

#ifndef DDTEXTTABLEITEMFIGURE_H
#define DDTEXTTABLEITEMFIGURE_H


#include <wx/arrstr.h>

#include "dd/wxhotdraw/figures/wxhdSimpleTextFigure.h"
#include "dd/dditems/utilities/ddDataType.h"

enum
{
	MNU_DDADDCOLUMN = 0,
	MNU_DELCOLUMN,
	MNU_RENAMECOLUMN,
	MNU_AUTONAMCOLUMN,
	MNU_NOTNULL,
	MNU_PKEY,
	MNU_UKEY,
	MNU_TYPESERIAL,
	MNU_TYPEBOOLEAN,
	MNU_TYPEINTEGER,
	MNU_TYPEMONEY,
	MNU_TYPEVARCHAR,
	MNU_TYPEOTHER,
	MNU_TYPEPKEY_CONSTRAINTNAME,
	MNU_TYPEUKEY_CONSTRAINTNAME,
	MNU_DELTABLE
};

class ddColumnFigure;
class ddTableFigure;

class ddTextTableItemFigure : public wxhdSimpleTextFigure
{
public:
	ddTextTableItemFigure(wxString &columnName, ddDataType dataType, ddColumnFigure *owner);
	~ddTextTableItemFigure();
	virtual wxString &getText(bool extended = false);
	wxString getType();
	virtual void setAlias(wxString alias);
	virtual wxString getAlias();
	virtual void setOneTimeNoAlias();
	virtual void createMenu(wxMenu &mnu);
	virtual const wxArrayString dataTypes();
	virtual void OnGenericPopupClick(wxCommandEvent &event, wxhdDrawingView *view);
	virtual void setText(wxString textString);
	virtual ddColumnFigure *getOwnerColumn();
	virtual void setOwnerColumn(ddColumnFigure *column);
	virtual void setShowDataType(bool value);
	wxhdITool *CreateFigureTool(wxhdDrawingEditor *editor, wxhdITool *defaultTool);
	virtual void displayBoxUpdate();
	int getTextWidth();
	int getTextHeight();
	ddDataType getDataType();
	void setDataType(ddDataType type);
	void setOwnerTable(ddTableFigure *table);  //only used by columns working as table title figure for setAlias method
	int getPrecision();
	int getScale();
protected:
	void setPrecision(int value);
	void setScale(int value);
	ddColumnFigure *ownerColumn;
	ddTableFigure *ownerTable;
	wxString colAlias;
private:
	ddDataType columnType;
	wxString out;
	bool showDataType;
	bool showAlias;
	bool oneTimeNoAlias;
	int precision;
	int scale;
};
#endif
