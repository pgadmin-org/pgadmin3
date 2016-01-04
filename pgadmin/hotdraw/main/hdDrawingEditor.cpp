//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// hdDrawingEditor.cpp - Main class that manages all other classes
//
//////////////////////////////////////////////////////////////////////////

#include "pgAdmin3.h"

// wxWindows headers
#include <wx/wx.h>
#include <wx/dcbuffer.h>
#include <wx/fontdlg.h>

// App headers
#include "hotdraw/main/hdDrawingEditor.h"
#include "hotdraw/main/hdDrawingView.h"
#include "hotdraw/tools/hdITool.h"
#include "hotdraw/figures/defaultAttributes/hdFontAttribute.h"

hdDrawingEditor::hdDrawingEditor(wxWindow *owner, bool defaultView)
{
	_diagrams = new hdArrayCollection();
	_model = new hdArrayCollection();
	editorOwner = owner;
	modelChanged = false;

	changeDefaultFiguresFont();
}

hdDrawingEditor::~hdDrawingEditor()
{
	//Hack: If just delete _diagrams collection an error is raised.
	hdDrawing *diagram;
	while(_diagrams->count() > 0)
	{
		diagram = (hdDrawing *) _diagrams->getItemAt(0);
		_diagrams->removeItemAt(0);
		delete diagram;
	}

	//Hack: If just delete _model collection an error is raised.
	hdIFigure *tmp;
	while(_model->count() > 0)
	{
		tmp = (hdIFigure *) _model->getItemAt(0);
		_model->removeItemAt(0);
		delete tmp;
	}

	if(_model)
		delete _model;

	if(_diagrams)
		delete _diagrams;
}

//Hack to allow create different kind of custom _views inside custom editor
hdDrawing *hdDrawingEditor::createDiagram(wxWindow *owner, bool fromXml)
{
	hdDrawing *_tmpModel = new hdDrawing(this);

	hdDrawingView *_viewTmp = new hdDrawingView(_diagrams->count(), owner, this, wxSize(1200, 1200), _tmpModel);

	// Set Scroll Bar & split
	_viewTmp->SetScrollbars( 10, 10, 127, 80 );
	_viewTmp->EnableScrolling(true, true);
	_viewTmp->AdjustScrollbars();

	_tmpModel->registerView(_viewTmp);

	//Add a new position inside each figure to allow use of this new diagram existing figures.
	int i;
	hdIFigure *tmp;
	if(!fromXml)
	{
		for(i = 0; i < _model->count(); i++)
		{
			tmp = (hdIFigure *) _model->getItemAt(i);
			tmp->AddPosForNewDiagram();
		}
	}

	//Add Diagram
	_diagrams->addItem((hdObject *) _tmpModel);
	modelChanged = true;
	return _tmpModel;
}

void hdDrawingEditor::deleteDiagram(int diagramIndex, bool deleteView)
{
	hdDrawing *_tmpModel = (hdDrawing *) _diagrams->getItemAt(diagramIndex);
	_diagrams->removeItemAt(diagramIndex);
	hdDrawingView *_viewTmp = _tmpModel->getView();
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
	hdIFigure *tmp;
	for(i = 0; i < _model->count(); i++)
	{
		tmp = (hdIFigure *) _model->getItemAt(i);
		tmp->RemovePosOfDiagram(diagramIndex);
	}
	modelChanged = true;
}

hdDrawingView *hdDrawingEditor::getExistingView(int diagramIndex)
{
	if(diagramIndex >= _diagrams->count() || diagramIndex < 0)
	{
		return NULL;
	}

	hdDrawing *diagram = (hdDrawing *) _diagrams->getItemAt(diagramIndex);
	return diagram->getView();
}

hdDrawing *hdDrawingEditor::getExistingDiagram(int diagramIndex)
{
	if(diagramIndex >= _diagrams->count() || diagramIndex < 0)
	{
		return NULL;
	}

	return (hdDrawing *) _diagrams->getItemAt(diagramIndex);
}

void hdDrawingEditor::addModelFigure(hdIFigure *figure)
{
	if(_model)
		_model->addItem(figure);
	modelChanged = true;
}

void hdDrawingEditor::removeFromAllSelections(hdIFigure *figure)
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

void hdDrawingEditor::deleteModelFigure(hdIFigure *figure)
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

void hdDrawingEditor::addDiagramFigure(int diagramIndex, hdIFigure *figure)
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

void hdDrawingEditor::removeDiagramFigure(int diagramIndex, hdIFigure *figure)
{
	getExistingDiagram(diagramIndex)->remove(figure);
	modelChanged = true;
}

bool hdDrawingEditor::modelIncludes(hdIFigure *figure)
{
	if(_model)
		return _model->existsObject(figure);
	return false;
}

int hdDrawingEditor::modelCount()
{
	if(_model)
		return _diagrams->count();
	return 0;
}

hdIteratorBase *hdDrawingEditor::modelFiguresEnumerator()
{
	return _model->createIterator();
}

hdIteratorBase *hdDrawingEditor::diagramsEnumerator()
{
	return _diagrams->createIterator();
}

void hdDrawingEditor::removeAllDiagramsFigures()
{
	int i, size = modelCount();

	for(i = 0; i < size ; i++)
	{
		getExistingDiagram(i)->removeAllFigures();
	}
	modelChanged = true;
}

void hdDrawingEditor::deleteAllModelFigures()
{
	removeAllDiagramsFigures();
	hdIFigure *tmp;
	while(_model->count() > 0)
	{
		tmp = (hdIFigure *) _model->getItemAt(0);
		_model->removeItemAt(0);
		delete tmp;
	}
	modelChanged = true;
}

void hdDrawingEditor::remOrDelSelFigures(int diagramIndex)
{
	int answer;
	hdIFigure *tmp;

	if (getExistingDiagram(diagramIndex)->countSelectedFigures() == 1)
	{
		tmp = (hdIFigure *) getExistingDiagram(diagramIndex)->selectedFigures()->getItemAt(0);
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

void hdDrawingEditor::changeDefaultFiguresFont()
{
	*hdFontAttribute::defaultFont = settings->GetDDFont();
}

bool hdDrawingEditor::modelHasChanged()
{
	return modelChanged;
}

void hdDrawingEditor::notifyChanged()
{
	modelChanged = true;
}
