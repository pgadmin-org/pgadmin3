//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2010, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// explainShape.cpp - Explain Shapes
//
//////////////////////////////////////////////////////////////////////////



// wxWindows headers
#include <wx/wx.h>

// App headers
#include "pgAdmin3.h"
#include "ctl/explainCanvas.h"

#include <wx/docview.h>

#include "images/ex_aggregate.xpm"
#include "images/ex_append.xpm"
#include "images/ex_bmp_heap.xpm"
#include "images/ex_bmp_index.xpm"
#include "images/ex_cte_scan.xpm"
#include "images/ex_group.xpm"
#include "images/ex_hash.xpm"
#include "images/ex_hash_anti_join.xpm"
#include "images/ex_hash_semi_join.xpm"
#include "images/ex_hash_setop_except.xpm"
#include "images/ex_hash_setop_except_all.xpm"
#include "images/ex_hash_setop_intersect.xpm"
#include "images/ex_hash_setop_intersect_all.xpm"
#include "images/ex_hash_setop_unknown.xpm"
#include "images/ex_index_scan.xpm"
#include "images/ex_join.xpm"
#include "images/ex_limit.xpm"
#include "images/ex_materialize.xpm"
#include "images/ex_merge.xpm"
#include "images/ex_merge_anti_join.xpm"
#include "images/ex_merge_semi_join.xpm"
#include "images/ex_nested.xpm"
#include "images/ex_nested_loop_anti_join.xpm"
#include "images/ex_nested_loop_semi_join.xpm"
#include "images/ex_recursive_union.xpm"
#include "images/ex_result.xpm"
#include "images/ex_scan.xpm"
#include "images/ex_seek.xpm"
#include "images/ex_setop.xpm"
#include "images/ex_sort.xpm"
#include "images/ex_subplan.xpm"
#include "images/ex_tid_scan.xpm"
#include "images/ex_unique.xpm"
#include "images/ex_unknown.xpm"
#include "images/ex_window_aggregate.xpm"
#include "images/ex_worktable_scan.xpm"

// Greenplum images
#include "images/ex_broadcast_motion.xpm"
#include "images/ex_redistribute_motion.xpm"
#include "images/ex_gather_motion.xpm"

#define BMP_BORDER 3

ExplainShape::ExplainShape(const char *bmp[], const wxString &description, long tokenNo, long detailNo)
{
	SetBitmap(wxBitmap(bmp));
	SetLabel(description, tokenNo, detailNo);
	kidCount = 0;
	totalShapes = 0;
	usedShapes = 0;
}


void ExplainShape::SetLabel(const wxString &str, int tokenNo, int detailNo)
{
	if (tokenNo < 0)
	{
		description = str;
		label = str;
	}
	else
	{
		wxStringTokenizer tokens(str, wxT(" "));

		while (tokenNo-- >= 0)
		{
			label = tokens.GetNextToken();

			if (detailNo <= 0)
			{
				if (!description.IsEmpty())
					description.Append(wxT(" "));

				description.Append(label);
			}
		}
		if (detailNo > 0)
		{
			tokens.SetString(str, wxT(" "));

			while (detailNo--)
			{
				if (!description.IsEmpty())
					description.Append(wxT(" "));

				description.Append(tokens.GetNextToken());
			}
			detail = tokens.GetString();
		}
	}
}

void ExplainShape::OnDraw(wxDC &dc)
{
	wxBitmap &bmp = GetBitmap();
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
	y += bmp.GetHeight() + BMP_BORDER;

	dc.DrawText(label, x, y);
}


void ExplainShape::OnLeftClick(double x, double y, int keys, int attachment)
{
	((ExplainCanvas *)GetCanvas())->ShowPopup(this);
}


#define ARROWMARGIN 5
wxRealPoint ExplainShape::GetStartPoint()
{
	wxRealPoint rp(GetX() + GetBitmap().GetWidth() / 2.0 + ARROWMARGIN, GetY() - (GetHeight() - GetBitmap().GetHeight()) / 2.);
	return rp;
}


wxRealPoint ExplainShape::GetEndPoint(int kidNo)
{
	wxRealPoint rp(GetX() - GetBitmap().GetWidth() / 2.0 - ARROWMARGIN, GetY() - (GetHeight() - GetBitmap().GetHeight()) / 2. + (kidCount > 1 ? GetBitmap().GetHeight() * 2. / 3. * kidNo / (2 * kidCount - 2) : 0 ));
	return rp;
}



ExplainShape *ExplainShape::Create(long level, ExplainShape *last, const wxString &str)
{
	ExplainShape *s = 0;

	int costPos = str.Find(wxT("(cost="));
	int actPos = str.Find(wxT("(actual"));

	wxStringTokenizer tokens(str, wxT(" "));
	wxString token = tokens.GetNextToken();
	wxString token2 = tokens.GetNextToken();
	wxString token3 = tokens.GetNextToken();
	wxString token4;
	if (tokens.HasMoreTokens())
		token4 = tokens.GetNextToken();
	wxString descr;
	if (costPos > 0)
		descr = str.Left(costPos);
	else if (actPos > 0)
		descr = str.Left(actPos);
	else
		descr = str;

	// possible keywords can be found in postgresql/src/backend/commands/explain.c

	if (token == wxT("Total"))              return 0;
	else if (token == wxT("Trigger"))       return 0;
	else if (token == wxT("Settings:"))		return 0;		/* Greenplum */
	else if (token == wxT("Slice"))			return 0;		/* Greenplum */
	else if (token.Mid(0, 6) == wxT("(slice"))	return 0;	/* Greenplum */
	else if (token == wxT("Result"))        s = new ExplainShape(ex_result_xpm, descr);
	else if (token == wxT("Append"))        s = new ExplainShape(ex_append_xpm, descr);
	else if (token == wxT("Nested"))
	{
		if (token2 == wxT("Loop") && token4 == wxT("Join"))
		{
			// Nested Loop Anti Join
			if (token3 == wxT("Anti"))
			{
				s = new ExplainShape(ex_nested_loop_anti_join_xpm, descr);
			}
			// Nested Loop Semi Join
			else
			{
				s = new ExplainShape(ex_nested_loop_semi_join_xpm, descr);
			}
		}
		if (!s)
			s = new ExplainShape(ex_nested_xpm, descr);
	}
	else if (token == wxT("Merge"))
	{
		if (token3 == wxT("Join"))
		{
			// Merge Anti Join
			if (token2 == wxT("Anti"))
			{
				s = new ExplainShape(ex_merge_anti_join_xpm, descr);
			}
			// Merge Semi Join
			else
			{
				s = new ExplainShape(ex_merge_semi_join_xpm, descr);
			}
		}
		else
		{
			s = new ExplainShape(ex_merge_xpm, descr);
		}
	}
	else if (token == wxT("Hash"))
	{
		if (token2 == wxT("Join"))
		{
			s = new ExplainShape(ex_join_xpm, descr);
		}
		else
		{
			if (token3 == wxT("Join"))
			{
				// Hash Anti Join
				if (token2 == wxT("Anti"))
				{
					s = new ExplainShape(ex_hash_anti_join_xpm, descr);
				}
				// Hash Semi Join
				else if (token2 == wxT("Semi"))
				{
					s = new ExplainShape(ex_hash_semi_join_xpm, descr);
				}
				else
				{
					s = new ExplainShape(ex_hash_xpm, descr);
				}
			}
			else
			{
				s = new ExplainShape(ex_hash_xpm, descr);
			}
		}
	}
	else if (token == wxT("HashSetOp"))
	{
		if (token2 == wxT("Except"))
		{
			// HashSetOp Except ALL
			if (token3 == wxT("ALL"))
			{
				s = new ExplainShape(ex_hash_setop_except_all_xpm, descr);
			}
			// HashSetOp Except
			else
			{
				s = new ExplainShape(ex_hash_setop_except_xpm, descr);
			}
		}
		else if (token2 == wxT("Intersect"))
		{
			// HashSetOp Intersect ALL
			if (token3 == wxT("ALL"))
			{
				s = new ExplainShape(ex_hash_setop_intersect_all_xpm, descr);
			}
			// HashSetOp Intersect
			else
			{
				s = new ExplainShape(ex_hash_setop_intersect_xpm, descr);
			}
		}
		else
		{
			// HashSetOp ???
			s = new ExplainShape(ex_hash_setop_unknown_xpm, descr);
		}
	}
	else if (token == wxT("Subquery"))      s = new ExplainShape(ex_subplan_xpm, descr, 0, 2);
	else if (token == wxT("Function"))      s = new ExplainShape(ex_result_xpm, descr, 0, 2);
	else if (token == wxT("Materialize"))   s = new ExplainShape(ex_materialize_xpm, descr);
	else if (token == wxT("Sort"))          s = new ExplainShape(ex_sort_xpm, descr);
	else if (token == wxT("Group"))         s = new ExplainShape(ex_group_xpm, descr);
	else if (token == wxT("Aggregate") || token == wxT("GroupAggregate") || token == wxT("HashAggregate"))
		s = new ExplainShape(ex_aggregate_xpm, descr);
	else if (token == wxT("Unique"))        s = new ExplainShape(ex_unique_xpm, descr);
	else if (token == wxT("SetOp"))         s = new ExplainShape(ex_setop_xpm, descr);
	else if (token == wxT("Limit"))         s = new ExplainShape(ex_limit_xpm, descr);
	else if (token == wxT("Bitmap"))
	{
		if (token2 == wxT("Index"))         s = new ExplainShape(ex_bmp_index_xpm, descr, 4, 3);
		else                                s = new ExplainShape(ex_bmp_heap_xpm, descr, 4, 3);
	}
	else if (token2 == wxT("Scan"))
	{
		if (token == wxT("Index"))
			// Scan Index Backword
			if (token3 == wxT("Backward"))
				s = new ExplainShape(ex_index_scan_xpm, descr, 4, 3);
			else
				s = new ExplainShape(ex_index_scan_xpm, descr, 3, 2);
		// Tid Scan
		else if (token == wxT("Tid"))
			s = new ExplainShape(ex_tid_scan_xpm, descr, 3, 2);
		// WorkTable scan
		else if (token == wxT("WorkTable"))
			s = new ExplainShape(ex_worktable_scan_xpm, descr, 3, 2);
		// CTE Scan
		else if (token == wxT("CTE"))
			s = new ExplainShape(ex_cte_scan_xpm, descr, 3, 2);
		else
			s = new ExplainShape(ex_scan_xpm, descr, 3, 2);
	}
	else if (token2 == wxT("Seek"))         s = new ExplainShape(ex_seek_xpm, descr, 3, 2);
	// Recursive Union
	else if (token == wxT("Recursive") && token2 == wxT("Union"))
		s = new ExplainShape(ex_recursive_union_xpm, descr);
	else if (token == wxT("WindowAgg"))
		s = new ExplainShape(ex_window_aggregate_xpm, descr);

	// Greenplum additions
	else if (token == wxT("Gather") && token2 == wxT("Motion"))
		s = new ExplainShape(ex_gather_motion_xpm, descr);
	else if (token == wxT("Broadcast") && token2 == wxT("Motion"))
		s = new ExplainShape(ex_broadcast_motion_xpm, descr);
	else if (token == wxT("Redistribute") && token2 == wxT("Motion"))
		s = new ExplainShape(ex_redistribute_motion_xpm, descr);

	if (!s)
		s = new ExplainShape(ex_unknown_xpm, descr);

	s->SetDraggable(false);

	s->level = level;

	if (costPos > 0)
	{
		if (actPos > 0)
		{
			s->actual = str.Mid(actPos);
			s->cost = str.Mid(costPos, actPos - costPos);
		}
		else
			s->cost = str.Mid(costPos);
	}
	else if (actPos > 0)
		s->actual = str.Mid(actPos);

	int w = 50, h = 20;

	wxBitmap &bmp = s->GetBitmap();
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
		wxChar *cl = const_cast<wxChar *>((const wxChar *)str + costPos + 6);
		wxChar *ch = wxStrstr(cl, wxT(".."));
		if (ch)
		{
			*ch = 0;
			ch += 2;
		}
		s->costLow = StrToDouble(cl);
		if (ch)
		{
			wxChar *r = wxStrstr(ch, wxT(" rows="));
			if (r)
			{
				*r = 0;
				r += 6;
			}
			s->costHigh = StrToDouble(ch);
			if (r)
			{
				wxChar *w = wxStrstr(r, wxT(" width="));
				if (w)
				{
					*w = 0;
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
	MakeLineControlPoints(4);

	width = (int) log(from->GetAverageCost());
	if (width > 10)
		width = 10;

	// If we got an average cost of 0, width is probably negative
	// which will look pretty darn ugly as an arrow width!
	// This may happen on Greenplum.
	if (width < 1)
		width = 1;

	wxNode *first = GetLineControlPoints()->GetFirst();
	wxNode *last  = GetLineControlPoints()->GetLast();
	*(wxRealPoint *)first->GetData() = from->GetStartPoint();
	*(wxRealPoint *)last->GetData() = to->GetEndPoint(from->GetKidno());

	wxRealPoint *p1 = (wxRealPoint *)first->GetNext()->GetData();
	wxRealPoint *p2 = (wxRealPoint *)last->GetPrevious()->GetData();
	*p1 = from->GetStartPoint();
	*p2 = to->GetEndPoint(from->GetKidno());
	p1->x -= (p1->x - p2->x) / 3. + 8;
	p2->x += (p1->x - p2->x) / 3. - 8;

	Initialise();
}


#define ARROWWIDTH  4


void ExplainLine::OnDraw(wxDC &dc)
{
	if (m_lineControlPoints)
	{
		dc.SetPen(*wxThePenList->FindOrCreatePen(*wxBLACK, 1, wxSOLID));
		dc.SetBrush(*wxTheBrushList->FindOrCreateBrush(*wxLIGHT_GREY, wxSOLID));

		wxPoint *points = new wxPoint[11];
		wxRealPoint *point0 = (wxRealPoint *) m_lineControlPoints->Item(0)->GetData();
		wxRealPoint *point1 = (wxRealPoint *) m_lineControlPoints->Item(1)->GetData();
		wxRealPoint *point2 = (wxRealPoint *) m_lineControlPoints->Item(2)->GetData();
		wxRealPoint *point3 = (wxRealPoint *) m_lineControlPoints->Item(3)->GetData();

		double phi  = atan2(point2->y - point1->y, point2->x - point1->x);
		double offs = width * tan(phi / 2);

		points[0].x  = WXROUND(point0->x);
		points[0].y  = WXROUND(point0->y) - width;
		points[10].x = WXROUND(point0->x);
		points[10].y = WXROUND(point0->y) + width;

		points[1].x  = WXROUND(point1->x + offs);
		points[1].y  = WXROUND(point1->y) - width;
		points[9].x  = WXROUND(point1->x - offs);
		points[9].y  = WXROUND(point1->y) + width;

		points[2].x  = WXROUND(point2->x + offs);
		points[2].y  = WXROUND(point2->y) - width;
		points[8].x  = WXROUND(point2->x - offs);
		points[8].y  = WXROUND(point2->y) + width;

		points[3].x  = WXROUND(point3->x) - width - ARROWWIDTH;
		points[3].y  = WXROUND(point3->y) - width;
		points[7].x  = WXROUND(point3->x) - width - ARROWWIDTH;
		points[7].y  = WXROUND(point3->y) + width;

		points[4].x  = points[3].x;
		points[4].y  = points[3].y - ARROWWIDTH;
		points[6].x  = points[7].x;
		points[6].y  = points[7].y + ARROWWIDTH;

		points[5].x  = WXROUND(point3->x);
		points[5].y  = WXROUND(point3->y);

		dc.DrawPolygon(11, points, 0, 0);
	}
}
