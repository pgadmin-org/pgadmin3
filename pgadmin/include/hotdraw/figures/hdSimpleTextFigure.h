//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// hdSimpleTextFigure.h - A simple rectangle  figure with text inside it
//
//////////////////////////////////////////////////////////////////////////

#ifndef HDSIMPLETEXTFIGURE_H
#define HDSIMPLETEXTFIGURE_H
#include "hotdraw/figures/hdAttributeFigure.h"

class hdDrawingView;

class hdSimpleTextFigure : public hdAttributeFigure
{
public:
	hdSimpleTextFigure(wxString textString);
	~hdSimpleTextFigure();
	virtual void setText(wxString textString);
	virtual wxString &getText(bool extended = false);
	virtual void setEditable(bool value);
	virtual bool getEditable();
	virtual void setFont(wxFont textFont);
	virtual int getPadding();
	void basicMoveBy(int posIdx, int x, int y);
	virtual void basicDraw(wxBufferedDC &context, hdDrawingView *view);
	virtual void basicDrawSelected(wxBufferedDC &context, hdDrawingView *view);
	virtual hdITool *CreateFigureTool(hdDrawingView *view, hdITool *defaultTool);
	virtual void createMenu(wxMenu &mnu) {}
	virtual void enablePopUp();
	virtual void disablePopUp();
	virtual bool menuEnabled();
	virtual void OnGenericPopupClick(wxCommandEvent &event, hdDrawingView *view);
protected:
	virtual void getFontMetrics(int &width, int &height);
	virtual void recalculateDisplayBox();
	wxArrayString strings;
	bool showMenu;
	bool textEditable;
private:
	int padding;
	wxString text;
	wxFont font;

};
#endif
