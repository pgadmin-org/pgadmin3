//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2005, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// explainShape.cpp - Explain Shapes
//
//////////////////////////////////////////////////////////////////////////



// wxWindows headers
#include <wx/wx.h>

// App headers
#include "pgAdmin3.h"

#include <wx/docview.h>
#include <explainCanvas.h>


#include "images/ex_limit.xpm"
class ExplainLimit : public ExplainShape
{
public:
    ExplainLimit(const wxString &str) { SetBitmap(wxBitmap(ex_limit_xpm)); SetLabel(str); }
};

#include "images/ex_unique.xpm"
class ExplainUnique : public ExplainShape
{
public:
    ExplainUnique(const wxString &str) { SetBitmap(wxBitmap(ex_unique_xpm)); SetLabel(str); }
};

#include "images/ex_append.xpm"
class ExplainAppend : public ExplainShape
{
public:
    ExplainAppend(const wxString &str) { SetBitmap(wxBitmap(ex_append_xpm)); SetLabel(str); }
};

#include "images/table.xpm"
class ExplainUnknown : public ExplainShape
{
public:
    ExplainUnknown(const wxString &str) { SetBitmap(wxBitmap(table_xpm)); SetLabel(str); }
};

#include "images/ex_hash.xpm"
class ExplainHash : public ExplainShape
{
public:
    ExplainHash(const wxString &str) { SetBitmap(wxBitmap(ex_hash_xpm)); SetLabel(str); }
};

#include "images/ex_nested.xpm"
class ExplainNestedLoop : public ExplainShape
{
public:
    ExplainNestedLoop(const wxString &str) { SetBitmap(wxBitmap(ex_nested_xpm)); SetLabel(str); }
};

#include "images/ex_merge.xpm"
class ExplainMerge : public ExplainShape
{
public:
    ExplainMerge(const wxString &str) { SetBitmap(wxBitmap(ex_merge_xpm)); SetLabel(str); }
};

#include "images/ex_join.xpm"
class ExplainHashJoin : public ExplainShape
{
public:
    ExplainHashJoin(const wxString &str) { SetBitmap(wxBitmap(ex_join_xpm)); SetLabel(str); }
};

#include "images/ex_aggregate.xpm"
class ExplainAggregate : public ExplainShape
{
public:
    ExplainAggregate(const wxString &str) { SetBitmap(wxBitmap(ex_aggregate_xpm)); SetLabel(str); }
};

#include "images/ex_scan.xpm"
class ExplainScan : public ExplainShape
{
public:
    ExplainScan(const wxString &str) { SetBitmap(wxBitmap(ex_scan_xpm)); SetLabel(str, 3); }
};

#include "images/ex_sort.xpm"
class ExplainSort : public ExplainShape
{
public:
    ExplainSort(const wxString &str) { SetBitmap(wxBitmap(ex_sort_xpm)); SetLabel(str); }
};

#include "images/ex_group.xpm"
class ExplainGroup : public ExplainShape
{
public:
    ExplainGroup(const wxString &str) { SetBitmap(wxBitmap(ex_group_xpm)); SetLabel(str); }
};

#include "images/ex_subplan.xpm"
class ExplainSubplan : public ExplainShape
{
public:
    ExplainSubplan(const wxString &str) { SetBitmap(wxBitmap(ex_subplan_xpm)); SetLabel(str); }
};

#include "images/ex_materialize.xpm"
class ExplainMaterialize : public ExplainShape
{
public:
    ExplainMaterialize(const wxString &str) { SetBitmap(wxBitmap(ex_materialize_xpm)); SetLabel(str); }
};

#include "images/ex_seek.xpm"
class ExplainSeek : public ExplainShape
{
public:
    ExplainSeek(const wxString &str) { SetBitmap(wxBitmap(ex_seek_xpm)); SetLabel(str, 3); }
};


#define BMP_BORDER 3

ExplainShape::ExplainShape()
{
    kidCount=0;
    totalShapes=0;
    usedShapes=0;
}


void ExplainShape::SetLabel(const wxString &str, int tokenNo)
{
    if (tokenNo < 0)
        label = str;
    else
    {
        wxStringTokenizer tokens(str, wxT(" "));
        while (tokenNo--)
            tokens.GetNextToken();
        label = tokens.GetNextToken();
    }
}

void ExplainShape::OnDraw(wxDC& dc)
{
    wxBitmap &bmp=GetBitmap();
    if (!bmp.Ok())
        return;

    int x, y;
    x = WXROUND(m_xpos - bmp.GetWidth() / 2.0);
    y = WXROUND(m_ypos - GetHeight() / 2.0);

    dc.DrawBitmap(bmp, x, y, true);

    int w, h;
    dc.SetFont(GetCanvas()->GetFont());
    dc.GetTextExtent(label, &w, &h);

    x = WXROUND(m_xpos - w / 2.0);
    y +=bmp.GetHeight() + BMP_BORDER;

    dc.DrawText(label, x, y);
}


void ExplainShape::OnLeftClick(double x, double y, int keys, int attachment)
{
    ((ExplainCanvas*)GetCanvas())->ShowPopup(this);
}


#define ARROWMARGIN 5
wxRealPoint ExplainShape::GetTopPoint()
{
#if EXPLAIN_VERTICAL
    wxRealPoint rp(GetX(), GetY() - GetHeight() / 2.0);
#else
    wxRealPoint rp(GetX() - GetBitmap().GetWidth() / 2.0 - ARROWMARGIN, GetY() - (GetHeight()-GetBitmap().GetHeight()) / 2.);
#endif
    return rp;
}


wxRealPoint ExplainShape::GetBottomPoint(int kidNo)
{
#if EXPLAIN_VERTICAL
    wxRealPoint rp(GetX() + (kidCount>1 ? GetBitmap().GetWidth() * 2. /3. * (kidNo-(kidCount-1)/2.) / (kidCount-1) : 0 ), GetY() + GetHeight()/2.);
#else
    wxRealPoint rp(GetX() + GetBitmap().GetWidth() / 2.0 + ARROWMARGIN, GetY() - (GetHeight()-GetBitmap().GetHeight()) / 2. + (kidCount>1 ? GetBitmap().GetHeight() * 2. /3. * kidNo / (2*kidCount-2) : 0 ));
#endif
    return rp;
}


ExplainShape *ExplainShape::Create(long level, ExplainShape *last, const wxString &str)
{
    ExplainShape *s=0;

    int costPos=str.Find(wxT("(cost="));

    wxStringTokenizer tokens(str, wxT(" "));
    wxString token = tokens.GetNextToken();
    wxString label = costPos > 0 ? str.Left(costPos) : str;


    if (token == wxT("Hash"))
    {
        token=tokens.GetNextToken();
        if (token == wxT("Join"))
            s = new ExplainHashJoin(label);
        else
        {
            token=tokens.GetNextToken();
            if (token == wxT("Join"))
                s = new ExplainHashJoin(label);
            else
                s = new ExplainHash(label);
        }
    }
    else if (token == wxT("Total"))         return 0;
    else if (token == wxT("Limit"))         s = new ExplainLimit(label);
    else if (token == wxT("Unique"))        s = new ExplainUnique(label);
    else if (token == wxT("Aggregate"))     s = new ExplainAggregate(label);
    else if (token == wxT("Append"))        s = new ExplainAppend(label);
    else if (token == wxT("Nested"))        s = new ExplainNestedLoop(label);
    else if (token == wxT("Merge"))         s = new ExplainMerge(label);
    else if (token == wxT("Materialize"))   s = new ExplainMaterialize(label);
    else if (token == wxT("Sort"))          s = new ExplainSort(label);
    else if (token == wxT("Group"))         s = new ExplainGroup(label);
    else if (token == wxT("Subquery"))      s = new ExplainSubplan(label);
    else if (token == wxT("Seq") || token == wxT("Index") || token == wxT("Tid") || token == wxT("Function"))
    {
        token = tokens.GetNextToken();
        if (token == wxT("Scan"))           s = new ExplainScan(label);
        else if (token == wxT("Seek"))      s = new ExplainSeek(label);
    }

    if (!s)
        s = new ExplainUnknown(label);
    
    s->SetDraggable(false);
    s->detail = str;
    s->level = level;

    if (costPos > 0)
    {
        int actPos = str.Find(wxT("(actual"));
        if (actPos > 0)
        {
            s->actual = str.Mid(actPos);
            s->cost = str.Mid(costPos, actPos-costPos);
        }
        else
            s->cost = str.Mid(costPos);
    }
    
    int w=50, h=20;

    wxBitmap &bmp=s->GetBitmap();
    if (w < bmp.GetWidth())
        w = bmp.GetWidth();

    s->SetHeight(bmp.GetHeight() + BMP_BORDER + h);
    s->SetWidth(w);

    s->upperShape = last;
    if (last)
    {
        s->kidNo = last->kidCount;
        last->kidCount++;
    }
    else
        s->kidNo = 0;

    if (costPos > 0)
    {
        wxChar *cl=(wxChar*)str.c_str() + costPos+6;
        wxChar *ch=wxStrstr(cl, wxT(".."));
        if (ch)
        {
            *ch=0;
            ch += 2;
        }
        s->costLow = StrToDouble(cl);
        if (ch)
        {
            wxChar *r=wxStrstr(ch, wxT(" rows="));
            if (r)
            {
                *r=0;
                r += 6;
            }
            s->costHigh = StrToDouble(ch);
            if (r)
            {
                wxChar *w=wxStrstr(r, wxT(" width="));
                if (w)
                {
                    *w=0;
                    w += 7;
                }
                s->rows = StrToLong(r);
                if (w)
                    s->width = StrToLong(w);
            }
        }
    }
    return s;
}



ExplainLine::ExplainLine(ExplainShape *from, ExplainShape *to, double weight)
{
    SetCanvas(from->GetCanvas());
    from->AddLine(this, to);
#if EXPLAIN_VERTICAL
    MakeLineControlPoints(2);
#else
    MakeLineControlPoints(4);
#endif
    AddArrow(ARROW_ARROW);

    double cost = log(from->GetAverageCost())*2;
    if (cost < 1.)
        cost=1.;
    if (cost > 20.)
        cost=20.;


    SetPen(wxThePenList->FindOrCreatePen(*wxBLACK, int(cost+.5), wxSOLID));

    wxNode *first = GetLineControlPoints()->GetFirst();
    wxNode *last  = GetLineControlPoints()->GetLast();
    *(wxRealPoint *)first->GetData() = from->GetTopPoint();
    *(wxRealPoint *)last->GetData() = to->GetBottomPoint(from->GetKidno());
#if EXPLAIN_VERTICAL
#else
    wxRealPoint *p1=(wxRealPoint *)first->GetNext()->GetData();
    wxRealPoint *p2=(wxRealPoint *)last->GetPrevious()->GetData();
    *p1 = from->GetTopPoint();
    *p2 = to->GetBottomPoint(from->GetKidno());
    p1->x -= (p1->x - p2->x)/3. -8;
    p2->x += (p1->x - p2->x)/3. +8;
#endif

    Initialise();
}


void ExplainLine::OnDraw(wxDC& dc)
{
    if (m_lineControlPoints)
    {
        dc.SetPen(*m_pen);

        int n = m_lineControlPoints->GetCount();
        wxPoint *points = new wxPoint[n];
        int i;
        for (i = 0; i < n; i++)
        {
            wxRealPoint* point = (wxRealPoint*) m_lineControlPoints->Item(i)->GetData();
            points[i].x = WXROUND(point->x);
            points[i].y = WXROUND(point->y);
        }
        dc.DrawLines(n, points);

#ifdef __WXMSW__
    // For some reason, last point isn't drawn under Windows.
        dc.DrawPoint(points[n-1]);
#endif

        delete[] points;

        wxPen *linePen=m_pen;
        SetPen(wxThePenList->FindOrCreatePen(m_pen->GetColour(), 1, wxSOLID));

        DrawArrows(dc);

        SetPen(linePen);
    }
}