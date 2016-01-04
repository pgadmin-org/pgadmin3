//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// ddColumnTextTool.h - Modification of simple text tool for editing composite figure columns
//
//////////////////////////////////////////////////////////////////////////

#ifndef DDCOLUMNTEXTTOOL_H
#define DDCOLUMNTEXTTOOL_H

#include "hotdraw/tools/hdSimpleTextTool.h"
#include "dd/dditems/figures/ddTextTableItemFigure.h"


class ddColumnTextTool : public hdSimpleTextTool
{

public:
	ddColumnTextTool(hdDrawingView *view, hdIFigure *fig, hdITool *dt,  bool fastEdit = true, wxString dialogCaption = wxEmptyString, wxString dialogMessage = wxEmptyString);
	~ddColumnTextTool();
	virtual void mouseDown(hdMouseEvent &event);  //Mouse Right Click
	virtual bool callDialog(hdDrawingView *view);
	// some events functions are needed but because a bug it were move to main View class as a hack.
protected:
private:
	ddTextTableItemFigure *colTextFigure;
	//Because a bug in the way wxwidgets connect events I can't declare it here, wxTextCtrl, instead I do it on the view.
};
#endif
