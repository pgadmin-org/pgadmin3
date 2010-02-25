//////////////////////////////////////////////////////////////////////////
//
// pgScript - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2010, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
//////////////////////////////////////////////////////////////////////////


#include "pgsTestSuite.h"

#include <wx/file.h>
#include "pgscript/generators/pgsDictionaryGen.h"

void pgsTestSuite::test_generator_dictionary(void)
{
	const wxString dictionary = wxT("cities.txt"); // More than 2 lines
	wxCSConv conv(wxT("utf-8")); // Encoding used for creating the dictionary
	
	// Content of the dictionary
	wxString example =	wxT("DixieWorld\n")
						wxT("SnoRite\n")
						wxT("SunGlo\n")
						wxT("Ozark Park\n")
						wxT("Like\n")
						wxT("Healthy Heights\n")
						wxT("Insure\n")
						wxT("First National State\n")
						wxT("AmeriBeach\n")
						wxT("Peaches N' More\n")
						wxT("Isle B Seaing U\n")
						wxT("Perpetua\n")
						wxT("Chi+\n")
						wxT("Town and Country\n")
						wxT("Sunny Farms\n")
						wxT("Midstate\n")
						wxT("Equus\n")
						wxT("Hot N' Humid\n")
						wxT("@lantia\n")
						wxT("Heritage Home\n")
						wxT("Bay Way\n")
						wxT("Aqua-rama\n")
						wxT("Lakes-a-plenty\n")
						wxT("Magnolia\n")
						wxT("Riverview\n")
						wxT("Skyview\n")
						wxT("Korn-O-copia\n")
						wxT("WinALot\n")
						wxT("Hard Rock State\n")
						wxT("Consolidated Peoples\n")
						wxT("I Can't Believe It's Not Mexico\n")
						wxT("DollarDaze\n")
						wxT("Phatlantic\n")
						wxT("Northland\n")
						wxT("South Lake Shores\n")
						wxT("Indiana\n")
						wxT("Ocean View\n")
						wxT("Gettysburg Estates\n")
						wxT("MicroState\n")
						wxT("Charlie\n")
						wxT("South Northland\n")
						wxT("Opry-Tune-Itee\n")
						wxT("Superultramax\n")
						wxT("Salterra\n")
						wxT("Ethan Allendale\n")
						wxT("Washington\n")
						wxT("Kountry Korner\n")
						wxT("Friendly Folks\n")
						wxT("DairyKween\n")
						wxT("Fin and Fur\n");
	
	// Create the dictionary
	wxFile dict_file;
	dict_file.Create(dictionary, true);
	TS_ASSERT(dict_file.IsOpened());
	dict_file.Write(example, conv);
	dict_file.Close();

	// Test with the provided dictionary
	{
		// [1] Test a *unique* random generator
		pgsDictionaryGen gen(dictionary, true, wxDateTime::GetTimeNow(), conv);
		wxString result;
		wxArrayString sav;
		for (int i = 0; i < gen.nb_lines(); i++)
		{
			result = gen.random();
			TS_ASSERT(sav.Index(result) == wxNOT_FOUND);
			sav.push_back(result);
		}
		TS_ASSERT(gen.random() == sav.Item(0));
		
		// [2] Test copy constructor
		pgsDictionaryGen cmp(gen);
		for (int i = 0; i < gen.nb_lines(); i++)
		{
			TS_ASSERT(gen.random() == cmp.random());
		}
	}
	
	// Test with an invalid dictionary
	{
		// [1] Test a random generator
		pgsDictionaryGen gen(wxT(""), false, wxDateTime::GetTimeNow(), conv);
		for (int i = 0; i < 10; i++)
		{
			TS_ASSERT(gen.random() == wxString());
		}
	
		// [2] Test copy constructor
		pgsDictionaryGen cmp(gen);
		for (int i = 0; i < 10; i++)
		{
			TS_ASSERT(gen.random() == cmp.random());
		}
	}
	
	// Test with the provided dictionary
	{
		// [1] Test a *non unique* random generator
		pgsDictionaryGen gen(dictionary, false, wxDateTime::GetTimeNow(), conv);
		wxString result;
		wxArrayString sav;
		for (int i = 0; i < gen.nb_lines() - 1; i++)
		{
			result = gen.random();
			sav.push_back(result);
		}
		// Should be true: the next value has already been generated once
		// TS_ASSERT(sav.Index(gen.random()) != wxNOT_FOUND);
		
		// [2] Test copy constructor
		pgsDictionaryGen cmp(gen);
		for (int i = 0; i < gen.nb_lines(); i++)
		{
			TS_ASSERT(gen.random() == cmp.random());
		}
	}
	
	// Test two generators with same seed
	{
		pgsDictionaryGen gen(dictionary, false, 0, conv);
		pgsDictionaryGen cmp(dictionary, false, 0, conv);
		wxString result;
		for (int i = 0; i < gen.nb_lines() * 2; i++)
		{
			result = gen.random();
			TS_ASSERT(result == cmp.random());
			TS_ASSERT(result != wxT(""));
		}
	}
	
	// Remove the dictionary
	::wxRemoveFile(dictionary);
}
