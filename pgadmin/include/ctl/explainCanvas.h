//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2007, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// explainCanvas.cpp - Explain Canvas
//
//////////////////////////////////////////////////////////////////////////

#ifndef EXPLAINCANVAS_H
#define EXPLAINCANVAS_H

#include <wx/ogl/ogl.h>


#if wxUSE_DEPRECATED
#error wxUSE_DEPRECATED should be 0!
#endif


class ExplainShape;
class ExplainPopup;
class ExplainText;

class ExplainCanvas : public wxShapeCanvas
{
public:
    ExplainCanvas(wxWindow *parent);
    ~ExplainCanvas();

    void ShowPopup(ExplainShape *s);
    void SetExplainString(const wxString &str);
    void Clear();

private:

    ExplainShape *rootShape, *lastShape;
    ExplainPopup *popup;
};


class ExplainShape : public wxBitmapShape
{
public:
    ExplainShape(char *bmp[], const wxString &description, long tokenNo=-1, long detailNo=-1);
    static ExplainShape *Create(long level, ExplainShape *last, const wxString &str); 

    void SetCondition(const wxString &str) { condition = str; }
    long GetLevel() { return level; }
    wxRealPoint GetStartPoint();
    wxRealPoint GetEndPoint(int kidNo);
    int GetKidno() { return kidNo; }

    ExplainShape *GetUpper() { return upperShape; }
    double GetAverageCost() { return (costHigh - costLow) / 2 + costLow; }

protected:
    void OnDraw(wxDC& dc);
    void OnLeftClick(double x, double y, int keys = 0, int attachment = 0);

    ExplainShape *upperShape;

    void SetLabel(const wxString &str, int tokenNo=-1, int detailNo=-1);

    long level;
    wxString description, detail, condition, label;
    wxString cost, actual;
    double costLow, costHigh;
    long rows, width;
    int kidCount, kidNo;
    int totalShapes; // horizontal space usage by shape and its kids
    int usedShapes;

    friend class ExplainCanvas;
    friend class ExplainText;
};


class ExplainLine : public wxLineShape
{
public:
    ExplainLine(ExplainShape *from, ExplainShape *to, double weight=0);

private:
    int width;
    void OnDraw(wxDC& dc);
};


class ExplainPopup : public wxDialog
{
public:
    ExplainPopup(wxWindow *w);
    void SetShape(ExplainShape *s);
    void Popup();

private:
    void OnMouseMove(wxMouseEvent &ev);

    ExplainText *explainText;
    wxPoint popupPoint;
    DECLARE_EVENT_TABLE()
};

#endif
