//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// hdDrawingView.cpp - Main canvas where all figures are drawn
//
//////////////////////////////////////////////////////////////////////////

#include "pgAdmin3.h"

// wxWindows headers
#include <wx/wx.h>
#include <wx/dcbuffer.h>

// App headers
#include "hotdraw/main/hdDrawingView.h"
#include "hotdraw/utilities/hdArrayCollection.h"
#include "hotdraw/main/hdDrawingEditor.h"
#include "hotdraw/utilities/hdGeometry.h"
#include "hotdraw/utilities/hdMouseEvent.h"
#include "hotdraw/tools/hdCanvasMenuTool.h"
#include "hotdraw/figures/defaultAttributes/hdFontAttribute.h"
#include "hotdraw/tools/hdSelectionTool.h"

// Images
#include "images/check.pngc"
#include "images/ddcancel.pngc"

BEGIN_EVENT_TABLE(hdDrawingView, wxScrolledWindow)
	EVT_PAINT(                     hdDrawingView::onPaint)
	EVT_MOTION(                    hdDrawingView::onMotion)
	EVT_RIGHT_DOWN(                hdDrawingView::onMouseDown)
	EVT_RIGHT_UP(                  hdDrawingView::onMouseUp)
	EVT_LEFT_DOWN(                 hdDrawingView::onMouseDown)
	EVT_LEFT_DCLICK(               hdDrawingView::onMouseDown)
	EVT_LEFT_UP(                   hdDrawingView::onMouseUp)
	EVT_ERASE_BACKGROUND(          hdDrawingView::onEraseBackGround)  //This erase flicker
	EVT_TEXT(CTL_TEXTTOOLID,       hdDrawingView::simpleTextToolChangeHandler)
	EVT_BUTTON(CTL_OKBUTTONID,     hdDrawingView::OnOkTxtButton)
	EVT_BUTTON(CTL_CANCELBUTTONID, hdDrawingView::OnCancelTxtButton)
	EVT_KEY_DOWN(				   hdDrawingView::onKeyDown)
	EVT_KEY_UP(					   hdDrawingView::onKeyUp)
END_EVENT_TABLE()


hdDrawingView::hdDrawingView(int diagram, wxWindow *ddParent, hdDrawingEditor *editor, wxSize size, hdDrawing *initialDrawing)// gqbController *controller, gqbModel *model)
	: wxScrolledWindow(ddParent, wxID_ANY, wxPoint(0, 0), size,
	                   wxHSCROLL | wxVSCROLL | wxBORDER | wxRETAINED)
{
	diagramIndex = diagram;
	drawing = initialDrawing;
	drawingEditor = editor;
	canvasSize = size;

#if wxCHECK_VERSION(2, 9, 0)
	FitInside();
#else
	SetVirtualSizeHints(canvasSize);
#endif

	// Hack to avoid selection rectangle drawing bug
	drawSelRect = false;
	// Hack to avoid event problem with simpleTextTool wxTextCrtl at EVT_TEXT event
	simpleTextToolEdit = new wxTextCtrl(this, CTL_TEXTTOOLID, wxT(""), wxDefaultPosition, wxDefaultSize, wxBORDER_NONE);
	simpleTextToolEdit->Hide();
	simpleTextFigure = NULL;
	menuFigure = NULL;
	okTxtButton = new wxBitmapButton(this, CTL_OKBUTTONID, *check_png_bmp, wxDefaultPosition, wxDefaultSize, wxBORDER_NONE);
	okTxtButton->Hide();
	cancelTxtButton = new wxBitmapButton(this, 1981, wxBitmap(*ddcancel_png_img), wxDefaultPosition, wxDefaultSize, wxBORDER_NONE);
	cancelTxtButton->Hide();
	canvasMenu = NULL;
	_tool = NULL;
	_tool = new hdSelectionTool(this);
}

hdDrawingView::~hdDrawingView()
{
	if(simpleTextToolEdit)
		delete simpleTextToolEdit;
	if(okTxtButton)
		delete okTxtButton;
	if(cancelTxtButton)
		delete cancelTxtButton;
	if(_tool)
		delete _tool;
}

void hdDrawingView::onPaint(wxPaintEvent &event)
{
	// Prepare Context for Buffered Draw
	wxPaintDC dcc(this);
	wxBufferedDC dc(&dcc, canvasSize);
	dc.Clear();
	hdIFigure *toDraw = NULL;
	hdIteratorBase *iterator = drawing->figuresEnumerator();

	while(iterator->HasNext())
	{
		toDraw = (hdIFigure *)iterator->Next();
		if(toDraw->isSelected(diagramIndex))
			toDraw->drawSelected(dc, this);
		else
			toDraw->draw(dc, this);
	}

	delete iterator;

	hdIHandle *tmpHandle = NULL;
	hdIteratorBase *selectionIterator = drawing->selectionFigures();//selection->createIterator();
	while(selectionIterator->HasNext())
	{
		toDraw = (hdIFigure *)selectionIterator->Next();
		hdIteratorBase *handlesIterator = toDraw->handlesEnumerator()->createIterator();
		while(handlesIterator->HasNext())
		{
			tmpHandle = (hdIHandle *)handlesIterator->Next();
			tmpHandle->draw(dc, this);
		}
		delete handlesIterator;
	}

	delete selectionIterator;

	//Hack to avoid selection rectangle drawing bug
	if (drawSelRect)
	{
		wxPen *pen = wxThePenList->FindOrCreatePen(*wxRED, 1, wxDOT);
		dc.SetPen(*pen);
		wxBrush *brush = wxTheBrushList->FindOrCreateBrush(*wxRED, wxTRANSPARENT);
		dc.SetBackground(*brush);
		dc.SetBackgroundMode(wxTRANSPARENT);
		//Adjust points before drawing
		wxPoint selAjustedPoints[5];
		CalcScrolledPosition(selPoints[0].x, selPoints[0].y, &selAjustedPoints[0].x, &selAjustedPoints[0].y);
		CalcScrolledPosition(selPoints[1].x, selPoints[1].y, &selAjustedPoints[1].x, &selAjustedPoints[1].y);
		CalcScrolledPosition(selPoints[2].x, selPoints[2].y, &selAjustedPoints[2].x, &selAjustedPoints[2].y);
		CalcScrolledPosition(selPoints[3].x, selPoints[3].y, &selAjustedPoints[3].x, &selAjustedPoints[3].y);
		CalcScrolledPosition(selPoints[4].x, selPoints[4].y, &selAjustedPoints[4].x, &selAjustedPoints[4].y);
		//Draw
		dc.DrawLines(5, selAjustedPoints, 0, 0);
		drawSelRect = false;
	}
}

//Hack to avoid selection rectangle drawing bug
void hdDrawingView::disableSelRectDraw()
{
	drawSelRect = false;
}

//Hack to avoid selection rectangle drawing bug
void hdDrawingView::setSelRect(hdRect &selectionRect)
{
	//Create rectangle lines to avoid non transparent brush for filling bug in wxwidgets
	selPoints[0].x = selectionRect.x;
	selPoints[0].y = selectionRect.y;
	selPoints[1].x = selectionRect.x + selectionRect.width;
	selPoints[1].y = selectionRect.y;
	selPoints[2].x = selectionRect.x + selectionRect.width;
	selPoints[2].y = selectionRect.y + selectionRect.height;
	selPoints[3].x = selectionRect.x;
	selPoints[3].y = selectionRect.y + selectionRect.height;
	selPoints[4].x = selectionRect.x;
	selPoints[4].y = selectionRect.y;
	drawSelRect = true;
}

// Overwrite and disable onEraseBackground Event to avoid Flicker
void hdDrawingView::onEraseBackGround(wxEraseEvent &event)
{
}

hdMultiPosRect hdDrawingView::getVisibleArea()
{
	int x, y, w, h;
	GetViewStart(&x, &y);
	GetClientSize(&w, &h);
	hdMultiPosRect visibleArea(x, y, w, h);
	return visibleArea;
}

hdMultiPosRect hdDrawingView::getVirtualSize()
{
	int w, h;
	GetVirtualSize(&w, &h);
	hdMultiPosRect virtualSize(0, 0, w, h);
	return virtualSize;
}

void hdDrawingView::ScrollToMakeVisible(hdPoint p)
{
	//implement this function
}

void hdDrawingView::ScrollToMakeVisible (hdRect r)
{
	//implement this function
}

hdIHandle *hdDrawingView::findHandle(int posIdx, double x, double y)
{
	hdIFigure *tmpFigure = NULL;
	hdIHandle *tmpHandle = NULL, *out = NULL;


	//Look for handles at each figure in SelectionEnumerator
	hdIteratorBase *selectionIterator = drawing->selectionFigures(); //selection->createIterator();
	while(selectionIterator->HasNext())
	{
		tmpFigure = (hdIFigure *)selectionIterator->Next();
		hdIteratorBase *handlesIterator = tmpFigure->handlesEnumerator()->createIterator();
		while(handlesIterator->HasNext())
		{
			tmpHandle = (hdIHandle *)handlesIterator->Next();
			if(tmpHandle->containsPoint(posIdx, x, y))
			{
				out = tmpHandle;
				break;
			}
		}
		delete handlesIterator;
	}
	delete selectionIterator;
	return out;
}

hdDrawing *hdDrawingView::getDrawing()
{
	return drawing;
}

void hdDrawingView::onMotion(wxMouseEvent &event)
{
// simple hack to don't update so frequently the canvas and mouse events
// Should be changed for a better one using time intervals not a simple counter
	static int simpleOptimization = 0;
	simpleOptimization++;
	if(simpleOptimization > 0)
	{
		simpleOptimization = 0;

		hdMouseEvent ddEvent = hdMouseEvent(event, this);
		if(event.Dragging())
		{
			_tool->mouseDrag(ddEvent);
			this->Refresh();			//only a dragging event on montion will change model
		}
		else
		{
			_tool->mouseMove(ddEvent);
		}
	}
}

void hdDrawingView::onMouseDown(wxMouseEvent &event)
{
	this->AcceptsFocus();
	this->SetFocus();
	startDrag = event.GetPosition();
	hdMouseEvent ddEvent = hdMouseEvent(event, this);
	_tool->mouseDown(ddEvent);
	this->Refresh();
}

void hdDrawingView::onMouseUp(wxMouseEvent &event)
{
	this->AcceptsFocus();
	this->SetFocus();
	hdMouseEvent ddEvent = hdMouseEvent(event, this);
	_tool->mouseUp(ddEvent);
	this->Refresh();
}

void hdDrawingView::onKeyDown(wxKeyEvent &event)
{
	hdKeyEvent ddEvent = hdKeyEvent(event, this);
	_tool->keyDown(ddEvent);
	this->Refresh();
}

void hdDrawingView::onKeyUp(wxKeyEvent &event)
{
	hdKeyEvent ddEvent = hdKeyEvent(event, this);
	_tool->keyUp(ddEvent);
	this->Refresh();
}

//Hack to avoid event problem with simpleTextTool wxTextCrtl at EVT_TEXT event
void hdDrawingView::setSimpleTextToolFigure(hdSimpleTextFigure *figure, bool onlySetFigure)
{
	simpleTextFigure = figure;
	menuFigure = NULL;
	if(simpleTextFigure && !onlySetFigure)
	{
		oldText = simpleTextFigure->getText();
		simpleTextToolEdit->SetValue(simpleTextFigure->getText());
		simpleTextToolEdit->SelectAll();
	}
}

//Hack to allow use (events) of wxmenu inside a tool Generic Way
void hdDrawingView::setMenuToolFigure(hdAbstractMenuFigure *figure)
{
	menuFigure = figure;
	simpleTextFigure = NULL;
}

//Hack to avoid event problem with simpleTextTool wxTextCrtl at EVT_TEXT event
void hdDrawingView::OnOkTxtButton(wxCommandEvent &event)
{
	_tool->deactivate(this);
	simpleTextToolEdit->Hide();
	okTxtButton->Hide();
	cancelTxtButton->Hide();
	setSimpleTextToolFigure(NULL);
}

//Hack to avoid event problem with simpleTextTool wxTextCrtl at EVT_TEXT event
void hdDrawingView::OnCancelTxtButton(wxCommandEvent &event)
{
	simpleTextToolEdit->SetValue(oldText);
	_tool->deactivate(this);
	simpleTextToolEdit->Hide();
	okTxtButton->Hide();
	cancelTxtButton->Hide();
	setSimpleTextToolFigure(NULL);
}

//Hack to avoid event problem with simpleTextTool wxTextCrtl at EVT_TEXT event (when text is set at edit it generate this event not sure ????)
void hdDrawingView::simpleTextToolChangeHandler(wxCommandEvent &event)
{
	if(!simpleTextToolEdit->IsModified() && simpleTextFigure)
	{
		simpleTextFigure->setText(simpleTextToolEdit->GetValue());
		//getFontMetrics
		int width, height;
		wxWindowDC dc(this);
		dc.SetFont(*hdFontAttribute::defaultFont);
		if(simpleTextFigure->getText(true).length() > 5)
			dc.GetTextExtent(simpleTextFigure->getText(true), &width, &height);
		else
			dc.GetTextExtent(wxT("EMPTY"), &width, &height);
		//recalculateDisplayBox
		hdGeometry g;
		simpleTextFigure->displayBox().width = g.max(width, 10) + simpleTextFigure->getPadding();
		simpleTextFigure->displayBox().height = g.max(height, 10) + simpleTextFigure->getPadding();
		//calculateSizeEntry
		hdPoint p = simpleTextFigure->displayBox().GetPosition(this->diagramIndex);
		CalcScrolledPosition(p.x, p.y, &p.x, &p.y);
		simpleTextToolEdit->SetPosition(p);
		simpleTextToolEdit->SetSize(simpleTextFigure->displayBox().GetSize());
		okTxtButton->SetPosition(wxPoint(p.x + simpleTextToolEdit->GetSize().GetWidth() + 4, p.y));
		cancelTxtButton->SetPosition(wxPoint(okTxtButton->GetPosition().x + okTxtButton->GetSize().GetWidth() + 4, p.y));
	}
	else if(!simpleTextFigure)
	{
		wxMessageDialog *error = new wxMessageDialog(NULL, wxT("Error locating hdSimpleTextTool figure"), wxT("Error!"), wxOK | wxICON_ERROR);
		error->ShowModal();
		delete error;
	}
	event.Skip();
}

//Hack to avoid event problem with simpleTextTool wxTextCrtl at EVT_TEXT event
wxTextCtrl *hdDrawingView::getSimpleTextToolEdit()
{
	return simpleTextToolEdit;
}

//Hack to avoid event problem with simpleTextTool wxTextCrtl at EVT_TEXT event
wxBitmapButton *hdDrawingView::getOkTxt()
{
	return okTxtButton;
}

//Hack to avoid event problem with simpleTextTool wxTextCrtl at EVT_TEXT event
wxBitmapButton *hdDrawingView::getCancelTxt()
{
	return cancelTxtButton;
}

//Hack to allow use (events) of wxmenu inside a tool like simpletexttool
void hdDrawingView::OnGenericPopupClick(wxCommandEvent &event)
{

	if(canvasMenu)
		canvasMenu->OnGenericPopupClick(event, this);
	else if(menuFigure)
		menuFigure->OnGenericPopupClick(event, this);
	else if(simpleTextFigure)
		simpleTextFigure->OnGenericPopupClick(event, this);

	simpleTextFigure = NULL;
	menuFigure = NULL;
	canvasMenu = NULL;

	event.Skip();
}

//Hack to allow use (events) of wxmenu inside a tool like simpletexttool
void hdDrawingView::connectPopUpMenu(wxMenu &mnu)
{
	// Connect the main menu
	mnu.Connect(wxEVT_COMMAND_MENU_SELECTED,
	            (wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction) &hdDrawingView::OnGenericPopupClick,
	            NULL,
	            this);

	// Connect all submenus
	wxMenuItem *item;
	wxMenuItemList list = mnu.GetMenuItems();
	for (unsigned int index = 0; index < list.GetCount(); index++)
	{
		wxMenuItemList::compatibility_iterator node = list.Item(index);
		item = (wxMenuItem *) node->GetData();
		if (item->IsSubMenu())
		{
			wxMenu *submenu = item->GetSubMenu();
			submenu->Connect(wxEVT_COMMAND_MENU_SELECTED,
			                 (wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction) &hdDrawingView::OnGenericPopupClick,
			                 NULL,
			                 this);
		}
	}
}

//Hack to allow use (events) of wxmenu inside a tool without a figure, Generic Way
void hdDrawingView::setCanvasMenuTool(hdCanvasMenuTool *menuTool)
{
	canvasMenu = menuTool;
}

hdDrawingEditor *hdDrawingView::editor()
{
	return drawingEditor;
}

bool hdDrawingView::AcceptsFocus() const
{
	return true;
}

void hdDrawingView::setTool(hdITool *tool)
{
	if(_tool)
		delete _tool;
	_tool = tool;
}

void hdDrawingView::createViewMenu(wxMenu &mnu)
{
	wxMenuItem *item;
	item = mnu.AppendCheckItem(1000, _("Sample Item"));
	item->Check(true);
}

void hdDrawingView::OnGenericViewPopupClick(wxCommandEvent &event)
{
	switch(event.GetId())
	{
		case 1000:
			wxMessageBox(_("Sample menu item"), _("Sample"), wxOK, this);
	}
}

void hdDrawingView::notifyChanged()
{
	drawingEditor->notifyChanged();
}
