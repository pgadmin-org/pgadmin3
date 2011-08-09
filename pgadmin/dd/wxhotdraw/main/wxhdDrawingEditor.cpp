//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2011, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// wxhdDrawingEditor.cpp - Main class that manages all other classes
//
//////////////////////////////////////////////////////////////////////////

#include "pgAdmin3.h"

// wxWindows headers
#include <wx/wx.h>
#include <wx/dcbuffer.h>
#include <wx/fontdlg.h>

// App headers
#include "dd/wxhotdraw/main/wxhdDrawingEditor.h"
#include "dd/wxhotdraw/main/wxhdDrawingView.h"
#include "dd/wxhotdraw/tools/wxhdITool.h"
#include "dd/wxhotdraw/figures/defaultAttributes/wxhdFontAttribute.h"

wxhdDrawingEditor::wxhdDrawingEditor(wxWindow *owner, bool defaultView)
{
	_diagrams = new wxhdArrayCollection();
	_model = new wxhdArrayCollection();
	editorOwner = owner;
	modelChanged = false;
}

wxhdDrawingEditor::~wxhdDrawingEditor()
{
	//Hack: If just delete _diagrams collection an error is raised.
	wxhdDrawing *diagram;
	while(_diagrams->count() > 0)
	{
		diagram = (wxhdDrawing *) _diagrams->getItemAt(0);
		_diagrams->removeItemAt(0);
		delete diagram;
	}

	//Hack: If just delete _model collection an error is raised.
	wxhdIFigure *tmp;
	while(_model->count() > 0)
	{
		tmp = (wxhdIFigure *) _model->getItemAt(0);
		_model->removeItemAt(0);
		delete tmp;
	}

	if(_model)
		delete _model;

	if(_diagrams)
		delete _diagrams;
}

//Hack to allow create different kind of custom _views inside custom editor
wxhdDrawing *wxhdDrawingEditor::createDiagram(wxWindow *owner, bool fromXml)
{
	wxhdDrawing *_tmpModel = new wxhdDrawing(this);

	wxhdDrawingView *_viewTmp = new wxhdDrawingView(_diagrams->count(), owner, this, wxSize(1200, 1200), _tmpModel);

	// Set Scroll Bar & split
	_viewTmp->SetScrollbars( 10, 10, 127, 80 );
	_viewTmp->EnableScrolling(true, true);
	_viewTmp->AdjustScrollbars();

	_tmpModel->registerView(_viewTmp);

	//Add a new position inside each figure to allow use of this new diagram existing figures.
	int i;
	wxhdIFigure *tmp;
	if(!fromXml)
	{
		for(i = 0; i < _model->count(); i++)
		{
			tmp = (wxhdIFigure *) _model->getItemAt(i);
			tmp->AddPosForNewDiagram();
		}
	}

	//Add Diagram
	_diagrams->addItem((wxhdObject *) _tmpModel);
	modelChanged = true;
	return _tmpModel;
}

void wxhdDrawingEditor::deleteDiagram(int diagramIndex, bool deleteView)
{
	wxhdDrawing *_tmpModel = (wxhdDrawing *) _diagrams->getItemAt(diagramIndex);
	_diagrams->removeItemAt(diagramIndex);
	wxhdDrawingView *_viewTmp = _tmpModel->getView();
	_tmpModel->registerView(NULL);
	if(_tmpModel)
		delete _tmpModel;
	if(_viewTmp && deleteView)
		delete _viewTmp;

	// Fix other diagrams positions
	int i;
	for(i = diagramIndex; i < _diagrams->count(); i++)
	{
		getExistingDiagram(i)->getView()->syncIdx(i);
	}

	//Remove this position inside each figure to allow delete that diagram related info from figures.
	wxhdIFigure *tmp;
	for(i = 0; i < _model->count(); i++)
	{
		tmp = (wxhdIFigure *) _model->getItemAt(i);
		tmp->RemovePosOfDiagram(diagramIndex);
	}
	modelChanged = true;
}

wxhdDrawingView *wxhdDrawingEditor::getExistingView(int diagramIndex)
{
	if(diagramIndex >= _diagrams->count() || diagramIndex < 0)
	{
		return NULL;
	}

	wxhdDrawing *diagram = (wxhdDrawing *) _diagrams->getItemAt(diagramIndex);
	return diagram->getView();
}

wxhdDrawing *wxhdDrawingEditor::getExistingDiagram(int diagramIndex)
{
	if(diagramIndex >= _diagrams->count() || diagramIndex < 0)
	{
		return NULL;
	}

	return (wxhdDrawing *) _diagrams->getItemAt(diagramIndex);
}

void wxhdDrawingEditor::addModelFigure(wxhdIFigure *figure)
{
	if(_model)
		_model->addItem(figure);
	modelChanged = true;
}

void wxhdDrawingEditor::removeFromAllSelections(wxhdIFigure *figure)
{
	if(_model)
	{
		if(_diagrams)
		{
			int i;
			for(i = 0; i < _diagrams->count(); i++)
			{
				if(getExistingDiagram(i)->isFigureSelected(figure))
					getExistingDiagram(i)->removeFromSelection(figure);
			}
		}
	}
}

void wxhdDrawingEditor::deleteModelFigure(wxhdIFigure *figure)
{
	if(_model)
	{
		if(_diagrams)
		{
			int i;
			for(i = 0; i < _diagrams->count(); i++)
			{
				if(getExistingDiagram(i)->includes(figure))
					getExistingDiagram(i)->remove(figure);
			}
		}
		_model->removeItem(figure);
		delete figure;
		figure = NULL;
	}
	modelChanged = true;
}

void wxhdDrawingEditor::addDiagramFigure(int diagramIndex, wxhdIFigure *figure)
{
	//first time figure is used at a diagram then add to it
	if(!modelIncludes(figure))
	{
		//Add figure to model
		addModelFigure(figure);
		//Add needed position(s) to figure to allow their use at new diagram(s).
		int i, start;
		start = figure->displayBox().CountPositions();
		for(i = start; i < _diagrams->count(); i++)
		{
			figure->AddPosForNewDiagram();
		}
	}
	getExistingDiagram(diagramIndex)->add(figure);
	modelChanged = true;
}

void wxhdDrawingEditor::removeDiagramFigure(int diagramIndex, wxhdIFigure *figure)
{
	getExistingDiagram(diagramIndex)->remove(figure);
	modelChanged = true;
}

bool wxhdDrawingEditor::modelIncludes(wxhdIFigure *figure)
{
	if(_model)
		return _model->existsObject(figure);
	return false;
}

int wxhdDrawingEditor::modelCount()
{
	if(_model)
		return _diagrams->count();
	return 0;
}

wxhdIteratorBase *wxhdDrawingEditor::modelFiguresEnumerator()
{
	return _model->createIterator();
}

wxhdIteratorBase *wxhdDrawingEditor::diagramsEnumerator()
{
	return _diagrams->createIterator();
}

void wxhdDrawingEditor::removeAllDiagramsFigures()
{
	int i, size = modelCount();

	for(i = 0; i < size ; i++)
	{
		getExistingDiagram(i)->removeAllFigures();
	}
	modelChanged = true;
}

void wxhdDrawingEditor::deleteAllModelFigures()
{
	removeAllDiagramsFigures();
	wxhdIFigure *tmp;
	while(_model->count() > 0)
	{
		tmp = (wxhdIFigure *) _model->getItemAt(0);
		_model->removeItemAt(0);
		delete tmp;
	}
	modelChanged = true;
}

void wxhdDrawingEditor::remOrDelSelFigures(int diagramIndex)
{
	int answer;
	wxhdIFigure *tmp;

	if (getExistingDiagram(diagramIndex)->countSelectedFigures() == 1)
	{
		tmp = (wxhdIFigure *) getExistingDiagram(diagramIndex)->selectedFigures()->getItemAt(0);
		answer = wxMessageBox(_("Are you sure you wish to delete figure ?"), _("Delete figures?"), wxYES_NO | wxNO_DEFAULT);
	}
	else if (getExistingDiagram(diagramIndex)->countSelectedFigures() > 1)
	{
		answer = wxMessageBox(
		             wxString::Format(_("Are you sure you wish to delete %d figures?"), getExistingDiagram(diagramIndex)->countSelectedFigures()),
		             _("Delete figures?"), wxYES_NO | wxNO_DEFAULT);
	}

	if (answer == wxYES)
	{
		while(getExistingDiagram(diagramIndex)->countSelectedFigures() > 0)
		{
			getExistingDiagram(diagramIndex)->removeFromSelection(tmp);
			getExistingDiagram(diagramIndex)->remove(tmp);
			if(tmp)
				delete tmp;
		}
		getExistingDiagram(diagramIndex)->clearSelection();  //reset selection to zero items
		modelChanged = true;
	}
}

void wxhdDrawingEditor::changeDefaultFiguresFont()
{
	wxhdFontAttribute::defaultFont = wxGetFontFromUser(editorOwner, wxhdFontAttribute::defaultFont, wxT("Select a default font for figures..."));
}

bool wxhdDrawingEditor::modelHasChanged()
{
	return modelChanged;
}

void wxhdDrawingEditor::notifyChanged()
{
	modelChanged = true;
}
