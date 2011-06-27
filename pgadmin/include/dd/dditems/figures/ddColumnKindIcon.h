//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2011, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// ddColumnKindIcon.h - Figure container for kind of Column Images
//
//////////////////////////////////////////////////////////////////////////

#ifndef DDCOLUMNKINDICON_H
#define DDCOLUMNKINDICON_H

#include "dd/wxhotdraw/figures/wxhdAbstractMenuFigure.h"

class ddColumnFigure;

enum ddColumnType {
	pk=0,
	uk,
	none,
	noaction  //no action kind means column type don't have right now a useful value
};

enum
{
    MNU_DDCTPKEY = 0,
    MNU_DDCTUKEY
};

class ddColumnKindIcon : public wxhdAbstractMenuFigure
{
public:
	ddColumnKindIcon(ddColumnFigure *owner);
    ~ddColumnKindIcon();
	virtual void OnGenericPopupClick(wxCommandEvent& event, wxhdDrawingView *view=NULL);
    virtual void createMenu(wxMenu &mnu);
	virtual void basicDraw(wxBufferedDC& context, wxhdDrawingView *view);
	virtual void basicDrawSelected(wxBufferedDC& context, wxhdDrawingView *view);		
	virtual void toggleColumnKind(ddColumnType type, wxhdDrawingView *view=NULL, bool interaction=true);
	virtual int getWidth();
	virtual int getHeight();
	bool isNone();
	bool isPrimaryKey();
	bool isForeignKey();
	void disableUniqueKey();
	void disablePrimaryKey();
	void enablePrimaryKey();
	ddColumnFigure* getOwnerColumn();
	bool isUniqueKey();
	bool isUniqueKey(int uniqueIndex);
	int getUniqueConstraintIndex();
	void setUniqueConstraintIndex(int i);
	void setRightIconForColumn();

protected:

private:
	ddColumnFigure *ownerColumn;
	wxBitmap *iconToDraw;
	wxBitmap icon;
	int ukIndex;
	bool isPk;

	//multiple Uk management at table
	void syncUkIndexes();
	bool uniqueConstraintManager(bool ukCol, wxhdDrawingView *view=NULL, bool interaction=true);
};
#endif
