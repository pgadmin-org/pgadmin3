//////////////////////////////////////////////////////////////////////////
//
// pgScript - PostgreSQL Tools
// 
// Copyright (C) 2002 - 2009, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
//////////////////////////////////////////////////////////////////////////


#include "pgsTestSuite.h"

#include "pgscript/expressions/pgsEqual.h"
#include "pgscript/expressions/pgsDifferent.h"
#include "pgscript/expressions/pgsGreater.h"
#include "pgscript/expressions/pgsGreaterEqual.h"
#include "pgscript/expressions/pgsLower.h"
#include "pgscript/expressions/pgsLowerEqual.h"
#include "pgscript/generators/pgsIntegerGen.h"
#include "pgscript/generators/pgsRealGen.h"
#include "pgscript/generators/pgsStringGen.h"
#include "pgscript/objects/pgsNumber.h"
#include "pgscript/objects/pgsRecord.h"
#include "pgscript/objects/pgsString.h"

void pgsTestSuite::test_object_record(void)
{
	const int max = +16383;
	const int min = -16384;
	const int nb_iterations = 10; // Must be greater than 3
	const USHORT nb_columns = 5; // Must be greater than 2
	
	pgsVarMap vars;
	
	// Test record with 0 column
	{
		pgsRecord rec(0);
		TS_ASSERT(!rec.insert(0, 0, pnew pgsString(wxT(""))));
		TS_ASSERT(rec.count_lines() == 1);
		TS_ASSERT(rec.count_columns() == 0);
	}

	// Test the string value of a record
	// Special characters must be escaped an each line must be returned
	// as a composite structure ("string val", 12345, ...)
	{
		pgsRecord rec(1);
		TS_ASSERT(rec.insert(0, 0, pnew pgsString(wxT("\"string\""))));
		TS_ASSERT(rec.value() == wxT("(\"\\\"string\\\"\")"));
		TS_ASSERT(rec.insert(1, 0, pnew pgsString(wxT("str\\i\nng"))));
		TS_ASSERT(rec.value() == wxT("(\"\\\"string\\\"\")\n(\"str\\\\i\nng\")"));
	}

	// Insert data and compare two records
	{
		// Prepare random generator to generate random data
		pgsIntegerGen int_gen(min, max, false);
		pgsRealGen real_gen(min, max, 3, false);
		pgsStringGen string_gen(10, 20, 1);
		
		
		for (int i = 0; i < nb_iterations; i++)
		{
			pgsRecord rec(nb_columns);

			// Basic TS_ASSERTs (no line, 'nb_columns' columns)
			{
				TS_ASSERT(rec.count_lines() == 0
						&& rec.count_columns() == nb_columns);
				TS_ASSERT(rec.is_record() && !rec.is_string()
						&& !rec.is_number());
			}

			// Get columns and elements (but there is nothing)
			{
				TS_ASSERT(rec.get_column(wxT(""))  == rec.count_columns());
				TS_ASSERT(rec.get_column(wxT("a")) == rec.count_columns());
				TS_ASSERT(rec.get(0, 0)->value() == wxT("")); // Nothing exists
				TS_ASSERT(rec.get(1, 0)->value() == wxT(""));
			}

			USHORT line = rand() % 9 + 1; // Index of the line (1 < line < 10)

			// Give names to columns and check them
			{
				TS_ASSERT(!rec.set_column_name(nb_columns, wxT("a")));
				for (USHORT j = 0; j < nb_columns; j++)
				{
					TS_ASSERT(rec.set_column_name(j, (wxString() << j)));
				}
				for (USHORT j = 0; j < nb_columns; j++)
				{
					TS_ASSERT(rec.get_column((wxString() << j)) == j);
				}
			}

			// Insert in a wrong location
			{
				TS_ASSERT(!rec.insert(0, nb_columns,
						pnew pgsString(int_gen.random())));
				TS_ASSERT(!rec.insert(0, wx_static_cast(USHORT, -1), 0));
				TS_ASSERT(!rec.insert(0, wx_static_cast(USHORT, -1),
						pnew pgsString(int_gen.random())));
			}

			// Insert random elements in the first 3 columns
			{
				// (1 <= line <= 10), 0
				TS_ASSERT(rec.insert(line, 0, pnew pgsNumber(int_gen.random(),
						pgsInt)));
				// (1 <= line <= 10), 1
				TS_ASSERT(rec.insert(line, 1, pnew pgsNumber(real_gen.random(),
						pgsReal)));
				// (1 <= line <= 10), 2
				TS_ASSERT(rec.insert(line, 2,
						pnew pgsString(string_gen.random())));
			}

			// Verify the number of lines and the number of columns
			// Verify that the first 3 columns contain something and not the other ones
			{
				TS_ASSERT(rec.count_lines()   == line + 1);
				TS_ASSERT(rec.count_columns() == nb_columns);
				for (USHORT i = 0; i < 3; i++)
				{
					TS_ASSERT(rec.get(line, rec.get_column((wxString()
							<< i)))->value() != wxT(""));
				}
				for (USHORT i = 3; i < nb_columns; i++)
				{
					TS_ASSERT(rec.get(line, rec.get_column((wxString()
							<< i)))->value() == wxT(""));
				}
			}
			
			// Assignment operator is forbidden
			// No test

			// Test the copy constructor
			pgsRecord copy(rec);

			// Check that the copy is equal with the original
			// pgsRecord way
			{
				TS_ASSERT(rec == copy && copy == rec);
				TS_ASSERT(rec <= copy && copy <= rec);
				TS_ASSERT(rec >= copy && copy >= rec);
				TS_ASSERT(!(rec < copy) && !(copy < rec));
				TS_ASSERT(!(rec > copy) && !(copy > rec));
				TS_ASSERT(rec.almost_equal(copy) && copy.almost_equal(rec));
			}
			
			// Check that the copy is equal with the original
			// pgsOperation way
			{
				pgsEqual eq(rec.clone(),         copy.clone());
				TS_ASSERT(eq.eval(vars)->value()  == wxT("1"));
				eq = pgsEqual(rec.clone(), copy.clone(), false);
				TS_ASSERT(eq.eval(vars)->value()  == wxT("1"));
				pgsDifferent neq(rec.clone(),    copy.clone());
				TS_ASSERT(neq.eval(vars)->value() == wxT("0"));
				pgsLowerEqual leq(rec.clone(),   copy.clone());
				TS_ASSERT(leq.eval(vars)->value() == wxT("1"));
				pgsGreaterEqual geq(rec.clone(), copy.clone());
				TS_ASSERT(geq.eval(vars)->value() == wxT("1"));
				pgsLower lo(rec.clone(),         copy.clone());
				TS_ASSERT(lo.eval(vars)->value()  == wxT("0"));
				pgsGreater gr(rec.clone(),       copy.clone());
				TS_ASSERT(gr.eval(vars)->value()  == wxT("0"));
			}
			
			wxString aux = rec.get(line, 2)->value();
			rec.insert(line, 2, pnew pgsString(aux.Upper()));
			
			// Test case sensitivity
			// pgsRecord way
			{
				TS_ASSERT(rec != copy && copy != rec);
				TS_ASSERT(!(rec <= copy) && !(copy <= rec));
				TS_ASSERT(!(rec >= copy) && !(copy >= rec));
				TS_ASSERT(!(rec < copy) && !(copy < rec));
				TS_ASSERT(!(rec > copy) && !(copy > rec));
				TS_ASSERT(rec.almost_equal(copy) && copy.almost_equal(rec));
			}
			
			// Test case sensitivity
			// pgsOperation way
			{
				pgsEqual eq(rec.clone(),         copy.clone());
				TS_ASSERT(eq.eval(vars)->value()  == wxT("0"));
				eq = pgsEqual(rec.clone(), copy.clone(), false);
				TS_ASSERT(eq.eval(vars)->value()  == wxT("1"));
			}
			
			rec.insert(line, 2, pnew pgsString(aux));

			// Add some elements to the copy
			{
				bool ins1 = copy.insert(0, 0, pnew pgsNumber(int_gen.random(),
						pgsInt));
				TS_ASSERT(ins1);
				bool ins2 = copy.insert(0, 1, pnew pgsNumber(real_gen.random(),
						pgsReal));
				TS_ASSERT(ins2);
				// Overwrites the previous element
				bool ins3 = copy.insert(0, 1,
						pnew pgsString(string_gen.random()));
				TS_ASSERT(ins3);
			}

			// New checks
			{
				TS_ASSERT(copy.count_lines()   == line + 1);
				TS_ASSERT(copy.count_columns() == nb_columns);
			}
			
			// Destroy copy automatically
		}
	}

	// Remove lines and compare two records
	{
		pgsIntegerGen int_gen(min, max, false);
		pgsRealGen real_gen(min, max, 3, false);
		pgsStringGen string_gen(10, 20, 1);

		pgsRecord rec(3);

		// Insert random data
		{
			for (int i = 0; i < nb_iterations; i++)
			{
				rec.insert(i, 0, pnew pgsString(int_gen.random()));
				rec.insert(i, 1, pnew pgsString(real_gen.random()));
				rec.insert(i, 2, pnew pgsString(string_gen.random()));
			}
		}

		// Check a few things
		{
			TS_ASSERT(rec.count_lines() == nb_iterations);
			TS_ASSERT(rec.get(nb_iterations - 1, 0)->value() != wxT(""));
		}

		// Create a fresh copy
		pgsRecord copy(rec);

		// Remove a line
		{
			TS_ASSERT(!copy.remove_line(nb_iterations));
			TS_ASSERT(copy.remove_line(1));
			TS_ASSERT(copy.count_lines() == nb_iterations - 1);
			TS_ASSERT(copy.get(nb_iterations - 1, 0)->value() == wxT(""));
		}

		// Comparisons (copy is included in rec)
		// Both pgsRecord and pgsOperation ways
		{
			TS_ASSERT(rec != copy && copy != rec);
			TS_ASSERT(!(rec <= copy) && copy <= rec);
			TS_ASSERT(rec >= copy && !(copy >= rec));
			TS_ASSERT(!(rec < copy) && copy < rec);
			TS_ASSERT(rec > copy && !(copy > rec));

			pgsEqual eq(rec.clone(),         copy.clone());
			TS_ASSERT(eq.eval(vars)->value()  == wxT("0"));
			pgsDifferent neq(rec.clone(),    copy.clone());
			TS_ASSERT(neq.eval(vars)->value() == wxT("1"));
			pgsLowerEqual leq(rec.clone(),   copy.clone());
			TS_ASSERT(leq.eval(vars)->value() == wxT("0"));
			pgsGreaterEqual geq(rec.clone(), copy.clone());
			TS_ASSERT(geq.eval(vars)->value() == wxT("1"));
			pgsLower lo(rec.clone(),         copy.clone());
			TS_ASSERT(lo.eval(vars)->value()  == wxT("0"));
			pgsGreater gr(rec.clone(),       copy.clone());
			TS_ASSERT(gr.eval(vars)->value()  == wxT("1"));
		}

		// Remove a line
		{
			TS_ASSERT(copy.remove_line(nb_iterations - 2));
			TS_ASSERT(copy.count_lines() == nb_iterations - 2);
			TS_ASSERT(copy.get(nb_iterations - 2, 0)->value() == wxT(""));
		}

		// Comparisons (copy should be included in rec)
		{
			TS_ASSERT(rec != copy && copy != rec);
			TS_ASSERT(!(rec <= copy) && copy <= rec);
			TS_ASSERT(rec >= copy && !(copy >= rec));
			TS_ASSERT(!(rec < copy) && copy < rec);
			TS_ASSERT(rec > copy && !(copy > rec));
		}

		// Remove everything
		// Copy should still be included in rec as it contains nothing
		{
			while (copy.count_lines() > 0)
			{
				TS_ASSERT(copy.remove_line(copy.count_lines() - 1));
			}
			
			TS_ASSERT(rec != copy && copy != rec);
			TS_ASSERT(!(rec <= copy) && copy <= rec);
			TS_ASSERT(rec >= copy && !(copy >= rec));
			TS_ASSERT(!(rec < copy) && copy < rec);
			TS_ASSERT(rec > copy && !(copy > rec));
		}
	}
	
	// Compare records
	{
		pgsRecord rec(2);
		pgsRecord cmp(2);
		
		// Nothing in the records: they should be equal
		{
			TS_ASSERT(rec == cmp && cmp == rec);
			TS_ASSERT(rec <= cmp && cmp <= rec);
			TS_ASSERT(rec >= cmp && cmp >= rec);
			TS_ASSERT(!(rec < cmp) && !(cmp < rec));
			TS_ASSERT(!(rec > cmp) && !(cmp > rec));
			TS_ASSERT(rec.almost_equal(cmp));
		}
		
		rec.insert(0, 0, pnew pgsString(wxT("a")));
		
		// cmp should be included in rec
		{
			TS_ASSERT(rec != cmp && cmp != rec);
			TS_ASSERT(!(rec <= cmp) && cmp <= rec);
			TS_ASSERT(rec >= cmp && !(cmp >= rec));
			TS_ASSERT(!(rec < cmp) && cmp < rec);
			TS_ASSERT(rec > cmp && !(cmp > rec));
			TS_ASSERT(!rec.almost_equal(cmp));
		}
		
		rec.insert(1, 0, pnew pgsString(wxT("b")));
		
		// cmp should be included in rec
		{
			TS_ASSERT(rec != cmp && cmp != rec);
			TS_ASSERT(!(rec <= cmp) && cmp <= rec);
			TS_ASSERT(rec >= cmp && !(cmp >= rec));
			TS_ASSERT(!(rec < cmp) && cmp < rec);
			TS_ASSERT(rec > cmp && !(cmp > rec));
			TS_ASSERT(!rec.almost_equal(cmp));
		}
		
		cmp.insert(0, 0, pnew pgsString(wxT("b")));
		
		// cmp should be included in rec
		{
			TS_ASSERT(rec != cmp && cmp != rec);
			TS_ASSERT(!(rec <= cmp) && cmp <= rec);
			TS_ASSERT(rec >= cmp && !(cmp >= rec));
			TS_ASSERT(!(rec < cmp) && cmp < rec);
			TS_ASSERT(rec > cmp && !(cmp > rec));
			TS_ASSERT(!rec.almost_equal(cmp));
		}
		
		cmp.insert(1, 0, pnew pgsString(wxT("A")));
		
		// cmp should be almost equal to rec (case insensitive)
		// The order does not matter
		{
			TS_ASSERT(rec != cmp && cmp != rec);
			TS_ASSERT(!(rec <= cmp) && !(cmp <= rec));
			TS_ASSERT(!(rec >= cmp) && !(cmp >= rec));
			TS_ASSERT(!(rec < cmp) && !(cmp < rec));
			TS_ASSERT(!(rec > cmp) && !(cmp > rec));
			TS_ASSERT(rec.almost_equal(cmp));
		}
		
		cmp.insert(1, 0, pnew pgsString(wxT("a")));
		
		// cmp should be equal to rec
		// The order does not matter
		{
			TS_ASSERT(rec == cmp && cmp == rec);
			TS_ASSERT(rec <= cmp && cmp <= rec);
			TS_ASSERT(rec >= cmp && cmp >= rec);
			TS_ASSERT(!(rec < cmp) && !(cmp < rec));
			TS_ASSERT(!(rec > cmp) && !(cmp > rec));
			TS_ASSERT(rec.almost_equal(cmp));
		}
		
		cmp.insert(2, 0, pnew pgsString(wxT("a")));
		
		// rec should be included in cmp
		{
			TS_ASSERT(rec != cmp && cmp != rec);
			TS_ASSERT(rec <= cmp && !(cmp <= rec));
			TS_ASSERT(!(rec >= cmp) && cmp >= rec);
			TS_ASSERT(rec < cmp && !(cmp < rec));
			TS_ASSERT(!(rec > cmp) && cmp > rec);
			TS_ASSERT(!rec.almost_equal(cmp));
		}
		
		cmp.remove_line(1);
		
		// cmp should be equal to rec
		// The order does not matter
		{
			TS_ASSERT(rec == cmp && cmp == rec);
			TS_ASSERT(rec <= cmp && cmp <= rec);
			TS_ASSERT(rec >= cmp && cmp >= rec);
			TS_ASSERT(!(rec < cmp) && !(cmp < rec));
			TS_ASSERT(!(rec > cmp) && !(cmp > rec));
			TS_ASSERT(rec.almost_equal(cmp));
		}
		
		// Test get_line
		{
			TS_ASSERT(rec.get_line(0)->value() != cmp.get_line(0)->value());
			TS_ASSERT(rec.get_line(0)->value() == cmp.get_line(1)->value());
		}
	}
}
