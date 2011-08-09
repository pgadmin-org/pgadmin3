//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2011, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// wxhdSimpleTextFigure.h - A simple rectangle  figure with text inside it
//
//////////////////////////////////////////////////////////////////////////

#ifndef WXHDSIMPLETEXTFIGURE_H
#define WXHDSIMPLETEXTFIGURE_H
#include "dd/wxhotdraw/figures/wxhdAttributeFigure.h"

class wxhdDrawingView;

class wxhdSimpleTextFigure : public wxhdAttributeFigure
{
public:
	wxhdSimpleTextFigure(wxString textString);
	~wxhdSimpleTextFigure();
	virtual void setText(wxString textString);
	virtual wxString &getText(bool extended = false);
	virtual void setEditable(bool value);
	virtual bool getEditable();
	virtual void setFont(wxFont textFont);
	virtual int getPadding();
	void basicMoveBy(int posIdx, int x, int y);
	virtual void basicDraw(wxBufferedDC &context, wxhdDrawingView *view);
	virtual void basicDrawSelected(wxBufferedDC &context, wxhdDrawingView *view);
	virtual wxhdITool *CreateFigureTool(wxhdDrawingView *view, wxhdITool *defaultTool);
	virtual void createMenu(wxMenu &mnu) {}
	virtual void enablePopUp();
	virtual void disablePopUp();
	virtual bool menuEnabled();
	virtual void OnGenericPopupClick(wxCommandEvent &event, wxhdDrawingView *view);
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
