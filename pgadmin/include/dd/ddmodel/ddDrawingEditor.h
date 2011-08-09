//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2011, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// wxhdDrawingEditor.h - Main class that manages all other classes
//
//////////////////////////////////////////////////////////////////////////

#ifndef DDDRAWINGEDITOR_H
#define DDDRAWINGEDITOR_H

#include "dd/wxhotdraw/main/wxhdDrawingEditor.h"
class frmDatabaseDesigner;

enum
{
	MNU_NEWTABLE = 321
};

class ddDatabaseDesign;

class ddDrawingEditor : public wxhdDrawingEditor
{
public:
	ddDrawingEditor(wxWindow *owner, wxWindow *frmOwner, ddDatabaseDesign *design);
	virtual wxhdDrawing *createDiagram(wxWindow *owner, bool fromXml);
	virtual void remOrDelSelFigures(int diagramIndex);
	void checkRelationshipsConsistency(int diagramIndex);
	void checkAllDigramsRelConsistency();
	ddDatabaseDesign *getDesign()
	{
		return databaseDesign;
	};
	virtual void notifyChanged();

protected:
private:
	ddDatabaseDesign *databaseDesign;
	frmDatabaseDesigner *frm;
};
#endif
