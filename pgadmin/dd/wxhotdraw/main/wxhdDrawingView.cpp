//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2011, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// wxhdDrawingView.cpp - Main canvas where all figures are drawn
//
//////////////////////////////////////////////////////////////////////////

#include "pgAdmin3.h"

// wxWindows headers
#include <wx/wx.h>
#include <wx/dcbuffer.h>

// App headers
#include "dd/wxhotdraw/main/wxhdDrawingView.h"
#include "dd/wxhotdraw/utilities/wxhdArrayCollection.h"
#include "dd/wxhotdraw/main/wxhdDrawingEditor.h"
#include "dd/wxhotdraw/utilities/wxhdGeometry.h"
#include "dd/wxhotdraw/utilities/wxhdMouseEvent.h"
#include "dd/wxhotdraw/tools/wxhdCanvasMenuTool.h"

// Images
#include "images/check.pngc"
#include "images/ddcancel.pngc"

BEGIN_EVENT_TABLE(wxhdDrawingView, wxScrolledWindow)
	EVT_PAINT(                     wxhdDrawingView::onPaint)
	EVT_MOTION(                    wxhdDrawingView::onMotion)
	EVT_RIGHT_DOWN(                wxhdDrawingView::onMouseDown)
	EVT_RIGHT_UP(                  wxhdDrawingView::onMouseUp)
	EVT_LEFT_DOWN(                 wxhdDrawingView::onMouseDown)
	EVT_LEFT_DCLICK(               wxhdDrawingView::onMouseDown)
	EVT_LEFT_UP(                   wxhdDrawingView::onMouseUp)
	EVT_ERASE_BACKGROUND(          wxhdDrawingView::onEraseBackGround)  //This erase flicker
	EVT_TEXT(CTL_TEXTTOOLID,       wxhdDrawingView::simpleTextToolChangeHandler)
	EVT_BUTTON(CTL_OKBUTTONID,     wxhdDrawingView::OnOkTxtButton)
	EVT_BUTTON(CTL_CANCELBUTTONID, wxhdDrawingView::OnCancelTxtButton)
	EVT_KEY_DOWN(				   wxhdDrawingView::onKeyDown)
	EVT_KEY_UP(					   wxhdDrawingView::onKeyUp)
END_EVENT_TABLE()


wxhdDrawingView::wxhdDrawingView(wxWindow *ddParent, wxhdDrawingEditor *editor, wxSize size, wxhdDrawing *initialDrawing)// gqbController *controller, gqbModel *model)
	: wxScrolledWindow(ddParent, wxID_ANY, wxPoint(0, 0), size,
	                   wxHSCROLL | wxVSCROLL | wxBORDER | wxRETAINED)
{
	drawing = initialDrawing;
	drawingEditor = editor;
	canvasSize = size;
	SetVirtualSizeHints(canvasSize);
	selection =  new wxhdCollection(new wxhdArrayCollection());
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
}

wxhdDrawingView::~wxhdDrawingView()
{
	if(selection)
	{
		selection->removeAll();
		delete selection;
	}
	if(simpleTextToolEdit)
		delete simpleTextToolEdit;
	if(okTxtButton)
		delete okTxtButton;
	if(cancelTxtButton)
		delete cancelTxtButton;
}

void wxhdDrawingView::onPaint(wxPaintEvent &event)
{
	// Prepare Context for Buffered Draw
	wxPaintDC dcc(this);
	wxBufferedDC dc(&dcc, canvasSize);
	dc.Clear();
	wxhdIFigure *toDraw = NULL;
	wxhdIteratorBase *iterator = drawing->figuresEnumerator();
	while(iterator->HasNext())
	{
		toDraw = (wxhdIFigure *)iterator->Next();
		if(toDraw->isSelected())
			toDraw->drawSelected(dc, this);
		else
			toDraw->draw(dc, this);
	}

	delete iterator;

	wxhdIHandle *tmpHandle = NULL;
	wxhdIteratorBase *selectionIterator = selection->createIterator();
	while(selectionIterator->HasNext())
	{
		toDraw = (wxhdIFigure *)selectionIterator->Next();
		wxhdIteratorBase *handlesIterator = toDraw->handlesEnumerator()->createIterator();
		while(handlesIterator->HasNext())
		{
			tmpHandle = (wxhdIHandle *)handlesIterator->Next();
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
void wxhdDrawingView::disableSelRectDraw()
{
	drawSelRect = false;
}

//Hack to avoid selection rectangle drawing bug
void wxhdDrawingView::setSelRect(wxhdRect &selectionRect)
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
void wxhdDrawingView::onEraseBackGround(wxEraseEvent &event)
{
}

void wxhdDrawingView::add(wxhdIFigure *figure)
{
	drawing->add(figure);
}


void wxhdDrawingView::remove(wxhdIFigure *figure)
{
	drawing->remove(figure);
}

void wxhdDrawingView::removeAll()
{
	selection->removeAll();
	drawing->deleteAllFigures();
}

void wxhdDrawingView::deleteSelectedFigures()
{
	int answer;
	wxhdIFigure *tmp;

	if (selection->count() == 1)
	{
		tmp = (wxhdIFigure *) selection->getItemAt(0);
		answer = wxMessageBox(_("Are you sure you wish to delete figure ?"), _("Delete figures?"), wxYES_NO | wxNO_DEFAULT);
	}
	else if (selection->count() > 1)
	{
		answer = wxMessageBox(
		             wxString::Format(_("Are you sure you wish to delete %d figures?"), selection->count()),
		             _("Delete figures?"), wxYES_NO | wxNO_DEFAULT);
	}

	if (answer == wxYES)
	{
		while(selection->count() > 0)
		{
			removeFromSelection(tmp);
			this->remove(tmp);
			if(tmp)
				delete tmp;
		}
		clearSelection();  //reset selection to zero items
	}
}

void wxhdDrawingView::addToSelection(wxhdIFigure *figure)
{
	if(!selection)
	{
		selection = new wxhdCollection(new wxhdArrayCollection());
	}
	if(figure)
	{
		figure->setSelected(true);
		selection->addItem(figure);
	}
}

void wxhdDrawingView::addToSelection(wxhdCollection *figures)
{
}

void wxhdDrawingView::removeFromSelection(wxhdIFigure *figure)
{
	figure->setSelected(false);
	if(selection)
		selection->removeItem(figure);
}


void wxhdDrawingView::toggleSelection(wxhdIFigure *figure)
{
	if(figure->isSelected() &&	selection->existsObject(figure))
		selection->removeItem(figure);
	else if(!figure->isSelected() && drawing->includes(figure))
		selection->addItem(figure);

	figure->setSelected(!figure->isSelected());
}

void wxhdDrawingView::clearSelection()
{
	wxhdIFigure *tmp = NULL;
	wxhdIteratorBase *iterator = selection->createIterator();
	while(iterator->HasNext())
	{
		tmp = (wxhdIFigure *)iterator->Next();
		tmp->setSelected(false);
	}
	selection->removeAll();
	delete iterator;
}

wxhdRect wxhdDrawingView::getVisibleArea()
{
	int x, y, w, h;
	GetViewStart(&x, &y);
	GetClientSize(&w, &h);
	wxhdRect visibleArea(x, y, w, h);
	return visibleArea;
}

wxhdRect wxhdDrawingView::getVirtualSize()
{
	int w, h;
	GetVirtualSize(&w, &h);
	wxhdRect virtualSize(0, 0, w, h);
	return virtualSize;
}

void wxhdDrawingView::ScrollToMakeVisible(wxhdPoint p)
{
	//implement this function
}

void wxhdDrawingView::ScrollToMakeVisible (wxhdRect r)
{
	//implement this function
}

wxhdIHandle *wxhdDrawingView::findHandle(double x, double y)
{
	wxhdIFigure *tmpFigure = NULL;
	wxhdIHandle *tmpHandle = NULL, *out = NULL;


	//Look for handles at each figure in SelectionEnumerator
	wxhdIteratorBase *selectionIterator = selection->createIterator();
	while(selectionIterator->HasNext())
	{
		tmpFigure = (wxhdIFigure *)selectionIterator->Next();
		wxhdIteratorBase *handlesIterator = tmpFigure->handlesEnumerator()->createIterator();
		while(handlesIterator->HasNext())
		{
			tmpHandle = (wxhdIHandle *)handlesIterator->Next();
			if(tmpHandle->containsPoint(x, y))
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

bool wxhdDrawingView::isFigureSelected(wxhdIFigure *figure)
{
	return selection->existsObject(figure);
}

wxhdIteratorBase *wxhdDrawingView::selectionFigures()
{
	return selection->createIterator();
}

wxhdDrawing *wxhdDrawingView::getDrawing()
{
	return drawing;
}

void wxhdDrawingView::onMotion(wxMouseEvent &event)
{
// simple hack to don't update so frequently the canvas and mouse events
// Should be changed for a better one using time intervals not a simple counter
	static int simpleOptimization = 0;
	simpleOptimization++;
	if(simpleOptimization > 0)
	{
		simpleOptimization = 0;

		wxhdMouseEvent ddEvent = wxhdMouseEvent(event, this);
		if(event.Dragging())
		{
			drawingEditor->tool()->mouseDrag(ddEvent);
			this->Refresh();			//only a dragging event on montion will change model
		}
		else
		{
			drawingEditor->tool()->mouseMove(ddEvent);
		}
	}
}

void wxhdDrawingView::onMouseDown(wxMouseEvent &event)
{
	this->AcceptsFocus();
	this->SetFocus();
	startDrag = event.GetPosition();
	wxhdMouseEvent ddEvent = wxhdMouseEvent(event, this);
	drawingEditor->tool()->mouseDown(ddEvent);
	this->Refresh();
}

void wxhdDrawingView::onMouseUp(wxMouseEvent &event)
{
	this->AcceptsFocus();
	this->SetFocus();
	wxhdMouseEvent ddEvent = wxhdMouseEvent(event, this);
	drawingEditor->tool()->mouseUp(ddEvent);
	this->Refresh();
}

void wxhdDrawingView::onKeyDown(wxKeyEvent &event)
{
	wxhdKeyEvent ddEvent = wxhdKeyEvent(event, this);
	drawingEditor->tool()->keyDown(ddEvent);
	this->Refresh();
}

void wxhdDrawingView::onKeyUp(wxKeyEvent &event)
{
	wxhdKeyEvent ddEvent = wxhdKeyEvent(event, this);
	drawingEditor->tool()->keyUp(ddEvent);
	this->Refresh();
}

//Hack to avoid event problem with simpleTextTool wxTextCrtl at EVT_TEXT event
void wxhdDrawingView::setSimpleTextToolFigure(wxhdSimpleTextFigure *figure, bool onlySetFigure)
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
void wxhdDrawingView::setMenuToolFigure(wxhdAbstractMenuFigure *figure)
{
	menuFigure = figure;
	simpleTextFigure = NULL;
}

//Hack to avoid event problem with simpleTextTool wxTextCrtl at EVT_TEXT event
void wxhdDrawingView::OnOkTxtButton(wxCommandEvent &event)
{
	drawingEditor->tool()->deactivate();
	simpleTextToolEdit->Hide();
	okTxtButton->Hide();
	cancelTxtButton->Hide();
	setSimpleTextToolFigure(NULL);
}

//Hack to avoid event problem with simpleTextTool wxTextCrtl at EVT_TEXT event
void wxhdDrawingView::OnCancelTxtButton(wxCommandEvent &event)
{
	simpleTextToolEdit->SetValue(oldText);
	drawingEditor->tool()->deactivate();
	simpleTextToolEdit->Hide();
	okTxtButton->Hide();
	cancelTxtButton->Hide();
	setSimpleTextToolFigure(NULL);
}

//Hack to avoid event problem with simpleTextTool wxTextCrtl at EVT_TEXT event (when text is set at edit it generate this event not sure ????)
void wxhdDrawingView::simpleTextToolChangeHandler(wxCommandEvent &event)
{
	if(!simpleTextToolEdit->IsModified() && simpleTextFigure)
	{
		simpleTextFigure->setText(simpleTextToolEdit->GetValue());
		//getFontMetrics
		int width, height;
		wxWindowDC dc(this);
		dc.SetFont(GetFont());
		if(simpleTextFigure->getText(true).length() > 5)
			dc.GetTextExtent(simpleTextFigure->getText(true), &width, &height);
		else
			dc.GetTextExtent(wxT("EMPTY"), &width, &height);
		//recalculateDisplayBox
		wxhdGeometry g;
		simpleTextFigure->displayBox().width = g.max(width, 10) + simpleTextFigure->getPadding();
		simpleTextFigure->displayBox().height = g.max(height, 10) + simpleTextFigure->getPadding();
		//calculateSizeEntry
		wxhdPoint p = simpleTextFigure->displayBox().GetPosition();
		CalcScrolledPosition(p.x, p.y, &p.x, &p.y);
		simpleTextToolEdit->SetPosition(p);
		simpleTextToolEdit->SetSize(simpleTextFigure->displayBox().GetSize());
		okTxtButton->SetPosition(wxPoint(p.x + simpleTextToolEdit->GetSize().GetWidth() + 4, p.y));
		cancelTxtButton->SetPosition(wxPoint(okTxtButton->GetPosition().x + okTxtButton->GetSize().GetWidth() + 4, p.y));
	}
	else if(!simpleTextFigure)
	{
		wxMessageDialog *error = new wxMessageDialog(NULL, wxT("Error locating wxhdSimpleTextTool figure"), wxT("Error!"), wxOK | wxICON_ERROR);
		error->ShowModal();
		delete error;
	}
	event.Skip();
}

//Hack to avoid event problem with simpleTextTool wxTextCrtl at EVT_TEXT event
wxTextCtrl *wxhdDrawingView::getSimpleTextToolEdit()
{
	return simpleTextToolEdit;
}

//Hack to avoid event problem with simpleTextTool wxTextCrtl at EVT_TEXT event
wxBitmapButton *wxhdDrawingView::getOkTxt()
{
	return okTxtButton;
}

//Hack to avoid event problem with simpleTextTool wxTextCrtl at EVT_TEXT event
wxBitmapButton *wxhdDrawingView::getCancelTxt()
{
	return cancelTxtButton;
}

//Hack to allow use (events) of wxmenu inside a tool like simpletexttool
void wxhdDrawingView::OnGenericPopupClick(wxCommandEvent &event)
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
void wxhdDrawingView::connectPopUpMenu(wxMenu &mnu)
{
	// Connect the main menu
	mnu.Connect(wxEVT_COMMAND_MENU_SELECTED,
	            (wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction) &wxhdDrawingView::OnGenericPopupClick,
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
			                 (wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction) &wxhdDrawingView::OnGenericPopupClick,
			                 NULL,
			                 this);
		}
	}
}

//Hack to allow use (events) of wxmenu inside a tool without a figure, Generic Way
void wxhdDrawingView::setCanvasMenuTool(wxhdCanvasMenuTool *menuTool)
{
	canvasMenu = menuTool;
}

wxhdDrawingEditor *wxhdDrawingView::editor()
{
	return drawingEditor;
}

bool wxhdDrawingView::AcceptsFocus() const
{
	return true;
}
