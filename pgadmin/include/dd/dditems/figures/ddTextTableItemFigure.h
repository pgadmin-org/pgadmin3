//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// ddTextTableItemFigure.h -
//
//////////////////////////////////////////////////////////////////////////

#ifndef DDTEXTTABLEITEMFIGURE_H
#define DDTEXTTABLEITEMFIGURE_H


#include <wx/arrstr.h>

#include "hotdraw/figures/hdSimpleTextFigure.h"
#include "dd/dditems/utilities/ddDataType.h"

enum
{
	MNU_DDADDCOLUMN = 321,
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

class ddTextTableItemFigure : public hdSimpleTextFigure
{
public:
	ddTextTableItemFigure(wxString &columnName, ddDataType dataType, ddColumnFigure *owner);
	~ddTextTableItemFigure();
	virtual wxString &getText(bool extended = false);
	wxString getType(bool raw = false);
	virtual void setAlias(wxString alias);
	virtual wxString getAlias();
	virtual void setOneTimeNoAlias();
	virtual void createMenu(wxMenu &mnu);
	virtual const wxArrayString dataTypes();
	virtual void OnGenericPopupClick(wxCommandEvent &event, hdDrawingView *view);
	virtual void setText(wxString textString);
	virtual ddColumnFigure *getOwnerColumn();
	virtual void setOwnerColumn(ddColumnFigure *column);
	virtual void setShowDataType(bool value);
	virtual bool getShowDataType()
	{
		return showDataType;
	};
	hdITool *CreateFigureTool(hdDrawingView *view, hdITool *defaultTool);
	virtual void displayBoxUpdate();
	void recalculateDisplayBox();
	int getTextWidth();
	int getTextHeight();
	ddDataType getDataType();
	void setDataType(ddDataType type);
	void setOwnerTable(ddTableFigure *table);  //only used by columns working as table title figure for setAlias method
	int getPrecision();
	int getScale();
	void setPrecision(int value);
	void setScale(int value);
protected:
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
