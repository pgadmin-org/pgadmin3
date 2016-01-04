//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// ddRelationshipFigure.h - Figure to draw foreign keys between tables.
//
//////////////////////////////////////////////////////////////////////////

#ifndef DDRELATIONSHIPFIGURE_H
#define DDRELATIONSHIPFIGURE_H
#include "hotdraw/figures/hdLineConnection.h"
#include "dd/dditems/figures/ddTableFigure.h"
#include "dd/dditems/figures/ddColumnFigure.h"

enum
{
	MNU_MANDATORYRELATIONSHIP = 321,
	MNU_IDENTIFYINGRELATIONSHIP,
	MNU_1MRELATIONSHIP,
	MNU_DELETERELATIONSHIP,
	MNU_FKCONSTRAINTNAME,
	MNU_FKMATCHTYPESIMPLE,
	MNU_FKMATCHTYPEFULL,
	MNU_FKONDELETENOACTION,
	MNU_FKONDELETERESTRICT,
	MNU_FKONDELETECASCADE,
	MNU_FKONDELETESETNULL,
	MNU_FKONDELETESETDEFAULT,
	MNU_FKONUPDATENOACTION,
	MNU_FKONUPDATERESTRICT,
	MNU_FKONUPDATECASCADE,
	MNU_FKONUPDATESETNULL,
	MNU_FKONUPDATESETDEFAULT,
	MNU_FKEYFROMPKEY,
	MNU_FKEYCUSTOMMAPPING,
	MNU_FKEYFROMUKEYBASE  //This constant should be always the last one to allow hack of multiple uk selection in submenu
};

enum actionKind
{
	FK_ACTION_NO = 600,
	FK_RESTRICT,
	FK_CASCADE,
	FK_SETNULL,
	FK_SETDEFAULT
};
class ddRelationshipItem;

WX_DECLARE_STRING_HASH_MAP( ddRelationshipItem *, columnsHashMap );


class ddRelationshipFigure : public hdLineConnection
{
public:
	ddRelationshipFigure();
	ddRelationshipFigure(int posIdx, hdIFigure *figure1, hdIFigure *figure2);
	~ddRelationshipFigure();
	virtual void createMenu(wxMenu &mnu);

	void prepareFkForDelete(ddColumnFigure *column);
	void updateForeignKey();
	void removeForeignKeys();
	void addExistingColumnFk(ddColumnFigure *startTablesourceCol, wxString destColumn);
	bool getIdentifying();
	bool getOneToMany();
	bool getMandatory();
	void setOptionAtForeignKeys(ddColumnOptionType type);
	void updatePkAtFkCols();
	bool isForeignKeyFromPk();
	virtual void connectEnd(hdIConnector *end, hdDrawingView *view = NULL);
	virtual void connectStart(hdIConnector *start, hdDrawingView *view = NULL);
	void disconnectStart(hdDrawingView *view = NULL);
	void disconnectEnd(hdDrawingView *view = NULL);
	void setFkFrom(bool primaryKey, int useUkIndex = -1, bool issueUpdateFk = false);
	wxString generateSQL(wxString schemaName);
	wxString getConstraintName();
	ddTableFigure *getStartTable();
	ddTableFigure *getEndTable();
	void changeFkOSTextColor(wxColour originalColour, wxColour fkColour, bool reset = false);
	int getUkIndex();
	actionKind getOnUpdateAction();
	actionKind getOnDeleteAction();
	bool getMatchSimple();
	columnsHashMap &getItemsHashMap()
	{
		return chm;
	};
	void initRelationValues(ddTableFigure *source, ddTableFigure *destination, int ukIdx, wxString constraint, actionKind onUpdate, actionKind onDelete, bool simpleMatch, bool identifying, bool oneToMany, bool mandatory, bool fromPk);

protected:
	virtual void basicDrawSelected(wxBufferedDC &context, hdDrawingView *view);
	virtual void basicDraw(wxBufferedDC &context, hdDrawingView *view);

private:
	virtual void OnGenericPopupClick(wxCommandEvent &event, hdDrawingView *view);
	bool fkFromPk;
	bool fkMandatory;
	bool fkOneToMany;
	bool fkIdentifying;
	bool matchSimple;
	bool paintingFkColumns;
	actionKind onUpdateAction, onDeleteAction;
	wxString constraintName;

	int ukIndex;
	columnsHashMap chm;
	ddTableFigure *disconnectedEndTable;
};

#endif
