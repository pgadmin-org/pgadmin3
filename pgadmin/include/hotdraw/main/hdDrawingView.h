//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// hdDrawingView.h - Main canvas where all figures are drawn
//
//////////////////////////////////////////////////////////////////////////

#ifndef HDDRAWINGVIEW_H
#define HDDRAWINGVIEW_H

#include "hotdraw/figures/hdIFigure.h"
#include "hotdraw/main/hdDrawing.h"
#include "hotdraw/handles/hdIHandle.h"
#include "hotdraw/figures/hdSimpleTextFigure.h"
#include "hotdraw/figures/hdAbstractMenuFigure.h"

class hdCanvasMenuTool;

// Create View Class for MVC pattern of graphic library of pgAdmin
enum
{
	CTL_TEXTTOOLID = 1979,
	CTL_OKBUTTONID,
	CTL_CANCELBUTTONID
};

class hdDrawingView : public wxScrolledWindow
{
public:
	hdDrawingView(int diagram, wxWindow *ddParent, hdDrawingEditor *editor , wxSize size, hdDrawing *drawing);
	~hdDrawingView();

	virtual void ScrollToMakeVisible(hdPoint p);
	virtual void ScrollToMakeVisible (hdRect r);
	virtual hdIHandle *findHandle(int posIdx, double x, double y);
	virtual hdDrawing *getDrawing();

	void onPaint(wxPaintEvent &event);
	void onEraseBackGround(wxEraseEvent &event);
	virtual void onMouseDown(wxMouseEvent &event);
	virtual void onMouseUp(wxMouseEvent &event);
	virtual void onMotion(wxMouseEvent &event);
	virtual void onKeyDown(wxKeyEvent &event);
	virtual void onKeyUp(wxKeyEvent &event);
	virtual hdMultiPosRect getVisibleArea();
	virtual hdMultiPosRect getVirtualSize();
	virtual bool AcceptsFocus () const;
	hdDrawingEditor *editor();
	wxSize canvasSize;

	//Hack To allow right click menu at canvas without a figure
	virtual void createViewMenu(wxMenu &mnu);
	virtual void OnGenericViewPopupClick(wxCommandEvent &event);
	//Hack to allow a different tool for each view
	hdITool *tool();
	void setTool(hdITool *tool);
	//Hack to avoid selection rectangle drawing bug
	void setSelRect(hdRect &selectionRect);
	void disableSelRectDraw();
	//Hack to avoid event problem with simpleTextTool wxTextCrtl at EVT_TEXT event
	void simpleTextToolChangeHandler(wxCommandEvent &event);
	void setSimpleTextToolFigure(hdSimpleTextFigure *figure, bool onlySetFigure = false);
	wxTextCtrl *getSimpleTextToolEdit();
	wxBitmapButton *getOkTxt();
	wxBitmapButton *getCancelTxt();
	//Hack to allow use (events) of wxmenu inside a tool Generic Way
	void setMenuToolFigure(hdAbstractMenuFigure *figure);
	//Hack to allow use (events) of wxmenu inside a tool like simpletexttool
	void OnGenericPopupClick(wxCommandEvent &event);
	void OnOkTxtButton(wxCommandEvent &event);
	void OnCancelTxtButton(wxCommandEvent &event);
	void connectPopUpMenu(wxMenu &mnu);
	//Hack to allow use (events) of wxmenu inside a tool without a figure, Generic Way
	void setCanvasMenuTool(hdCanvasMenuTool *menuTool);
	//Hack to allow use of a figure into multiple diagrams.
	int getIdx()
	{
		return diagramIndex;
	}
	void syncIdx(int newDiagramIndex)
	{
		diagramIndex = newDiagramIndex;
	};
	void notifyChanged();
protected:
	int diagramIndex;
private:
	DECLARE_EVENT_TABLE()
	hdDrawing *drawing;
	hdDrawingEditor *drawingEditor;

	//Hack to allow a different tool for each view
	hdITool *_tool;

	//Hack to allow auto scrolling when dragging mouse.
	hdPoint startDrag;

	//Hack to avoid selection rectangle drawing bug
	hdRect selRect;
	wxPoint selPoints[5];
	bool drawSelRect;
	//End Hack to avoid selection rectangle drawing bug

	//Hack to allow use (events) of wxmenu inside a tool with a figure, Generic Way
	hdAbstractMenuFigure *menuFigure;
	//End hack to allow use (events) of wxmenu inside a tool with a figure, Generic Way

	//Hack to avoid event problem with simpleTextTool wxTextCrtl at EVT_TEXT event && POPUP EVENT
	wxTextCtrl *simpleTextToolEdit;
	wxBitmapButton *okTxtButton, *cancelTxtButton;
	hdSimpleTextFigure *simpleTextFigure;
	wxString oldText;
	//End Hack to avoid event problem with simpleTextTool wxTextCrtl at EVT_TEXT event && POPUP EVENT

	//Hack to allow use (events) of wxmenu inside a tool without a figure, Generic Way
	hdCanvasMenuTool *canvasMenu;
	//Hack to allow use (events) of wxmenu inside a tool without a figure, Generic Way
};
#endif
