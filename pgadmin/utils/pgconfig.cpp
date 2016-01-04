//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// pgconfig.cpp - backend configuration classes
//
//////////////////////////////////////////////////////////////////////////

#include "pgAdmin3.h"

#include <wx/textbuf.h>

#include "utils/pgconfig.h"
#include "db/pgConn.h"
#include "db/pgSet.h"
#include "utils/utffile.h"


#define DEFAULT_COMMENT_INDEX       wxT("\t\t# ")


int FindToken(const wxString &str, const wxChar **list)
{
	int index;
	for (index = 0 ; list[index] ; index++)
	{
		if (list[index] == str)
			return index;

	}
	return -1;
}


const wxChar *pgConfigTypeStrings[] =
{
	wxT("bool"),
	wxT("integer"),
	wxT("real"),
	wxT("string"),
	0
};


void pgSettingItem::SetType(const wxString &str)
{
	int index = FindToken(str, pgConfigTypeStrings);
	if (index < 0)
		type = PGC_STRING;
	else
		type = (pgConfigType)index;
}


const wxChar *pgConfigContextStrings[] =
{
	wxT("internal"),
	wxT("postmaster"),
	wxT("sighup"),
	wxT("backend"),
	wxT("superuser"),
	wxT("userlimit"),
	wxT("user"),
	0
};


void pgSettingItem::SetContext(const wxString &str)
{
	int index = FindToken(str, pgConfigContextStrings);
	if (index < 0)
		context = PGC_INTERNAL;
	else
		context = (pgConfigContext)index;
}


const wxChar *pgConfigSourceStrings[] =
{
	wxT("default"),
	wxT("environment variable"),
	wxT("configuration file"),
	wxT("command line"),
	wxT("unprivileged"),
	wxT("database"),
	wxT("user"),
	wxT("client"),
	wxT("override"),
	wxT("interactive"),
	wxT("test"),
	wxT("session"),
	0
};


void pgSettingItem::SetSource(const wxString &str)
{
	int index = FindToken(str, pgConfigSourceStrings);
	if (index < 0)
		source = PGC_UNKNOWNSOURCE;
	else
		source = (pgConfigSource)index;
}


wxString pgSettingItem::GetActiveValue()
{
	if (newLine)
	{
		if (!newLine->isComment)
			return newLine->value;
	}
	else if (orgLine)
	{
		if (!orgLine->isComment)
			return orgLine->value;
	}
	return value;
}


pgConfigLine::pgConfigLine(pgConfigLine *line)
{
	item = line->item;
	value = line->value;
	comment = line->comment;
	isComment = line->isComment;
}


bool pgConfigLine::Differs(pgConfigLine *line)
{
	return value != line->value || comment != line->comment || isComment != line->isComment;
}


wxString pgConfigLine::GetNewText()
{
	wxString quote;
	wxString newLine;
	if (item->type == pgSettingItem::PGC_STRING)
	{
		if (value.Find('\'') >= 0)
			quote = wxT("\"");
		else
			quote = wxT("'");
	}
	if (isComment)
		newLine = wxT("# ");

	newLine += item->name + wxT(" = ")
	           + quote + value + quote;

	if (!comment.IsEmpty())
	{
		if (item->orgLine && item->orgLine->commentIndent.Find('#') >= 0)
			newLine += item->orgLine->commentIndent;
		else
			newLine += DEFAULT_COMMENT_INDEX;
		newLine += comment;
	}
	return newLine;
}


pgConfigOrgLine::pgConfigOrgLine(pgConfigLine *line) : pgConfigLine(line)
{
	text = line->GetNewText();
	if (isComment)
		commentIndent = DEFAULT_COMMENT_INDEX;
}


wxString pgConfigOrgLine::GetNewText()
{
	if (!item || !item->newLine)
		return text;
	return item->newLine->GetNewText();
}


////////////////////////////////////////////////


pgSettingFileReader::pgSettingFileReader(bool localized)
{

	wxUtfFile file;

	wxString path = i18nPath + wxT("/") + settings->GetCanonicalLanguageName() + wxT("/pg_settings.csv");
	if (localized && wxFile::Exists(path))
		file.Open(path);
	else
		file.Open(i18nPath + wxT("/pg_settings.csv"));

	if (file.IsOpened())
	{
		file.Read(buffer);
		file.Close();
	}

	if (!buffer.IsEmpty())
	{
		buffer = wxTextBuffer::Translate(buffer, wxTextFileType_Unix);

		columnNames = buffer.BeforeFirst('\n');
		bp = const_cast<wxChar *>((const wxChar *)buffer + columnNames.Length() + 1);
	}
}


pgSettingFileReader::~pgSettingFileReader()
{
}


pgSettingItem *pgSettingFileReader::GetNextItem()
{
	if (!bp || !*bp || *bp == '\n')
		return 0;

	pgSettingItem *item = new pgSettingItem;
	wxStringTokenizer tk(columnNames, wxT(";"));
	wxString column = tk.GetNextToken();

	wxChar *c = bp;
	while (*c)
	{
		wxString value;

		if (*c++ != '"')
		{
			// invalid syntax
			return 0;
		}
		while (*c && *c != '\n')
		{
			if (*c == '"')
			{
				if (c[1] == '"')
					c++;
				else
					break;
			}
			value.Append(*c++);
		}
		if (*c++ != '"')
		{
			// format error
			return 0;
		}
		if (column == wxT("name") || column == wxT("\"name\""))
			item->name = value.Lower();
		else if (column == wxT("category") || column == wxT("\"category\""))
			item->category = value;
		else if (column == wxT("short_desc") || column == wxT("\"short_desc\""))
			item->short_desc = value;
		else if (column == wxT("extra_desc") || column == wxT("\"extra_desc\""))
			item->extra_desc = value;
		else if (column == wxT("min_val") || column == wxT("\"min_val\""))
			item->min_val = value;
		else if (column == wxT("max_val") || column == wxT("\"max_val\""))
			item->max_val = value;
		else if (column == wxT("context") || column == wxT("\"context\""))
			item->SetContext(value);
		else if (column == wxT("vartype") || column == wxT("\"vartype\""))
			item->SetType(value);

		column = tk.GetNextToken();
		if (*c == '\n')
		{
			bp = c + 1;
			break;
		}
		else if (!*c)
		{
			bp = 0;
			break;
		}
		else if (*c == ';')
			c++;
		else
		{
			// invalid syntax
			return 0;
		}
	}

	return item;
}


pgSettingDbReader::pgSettingDbReader(pgConn *conn)
{
	set = conn->ExecuteSet(wxT("SELECT name, setting, source, category, short_desc, extra_desc, context, vartype, min_val, max_val FROM pg_settings ORDER BY name"));
}


pgSettingDbReader::~pgSettingDbReader()
{
	if (set)
		delete set;
}

pgSettingItem *pgSettingDbReader::GetNextItem()
{
	if (set->Eof())
		return 0;

	pgSettingItem *item = new pgSettingItem;

	item->name = set->GetVal(wxT("name")).Lower();
	item->category = set->GetVal(wxT("category"));
	item->short_desc = set->GetVal(wxT("short_desc"));
	item->extra_desc = set->GetVal(wxT("extra_desc"));
	item->min_val = set->GetVal(wxT("min_val"));
	item->max_val = set->GetVal(wxT("max_val"));
	item->SetContext(set->GetVal(wxT("context")));
	item->SetType(set->GetVal(wxT("vartype")));
	item->SetSource(set->GetVal(wxT("source")));
	item->value = set->GetVal(wxT("setting"));

	set->MoveNext();

	return item;
}


///////////////////////////////////////////////////////

// must match enum pgHbaConnectType!!!
const wxChar *pgHbaConnectTypeStrings[] =
{
	wxT("local"),
	wxT("host"),
	wxT("hostssl"),
	wxT("hostnossl"),
	0
};


// must match enum pgHbaMethod!!!
const wxChar *pgHbaMethodStrings[] =
{
	wxT("trust"),
	wxT("reject"),
	wxT("md5"),
	wxT("crypt"),
	wxT("password"),
	wxT("krb4"),
	wxT("krb5"),
	wxT("ident"),
	wxT("pam"),
	wxT("ldap"),
	wxT("gss"),
	wxT("sspi"),
	wxT("cert"),
	wxT("peer"),
	wxT("radius"),
	0
};


bool IsSpaceChar(wxChar c, const wxChar *spaceChars = wxT("\t "))
{
	return wxStrchr(spaceChars, c) != NULL;
}


void SkipSpace(const wxChar *&ptr, const wxChar *spaceChars = wxT("\t "))
{
	while (*ptr && IsSpaceChar(*ptr))
		ptr++;
}


void SkipNonspace(const wxChar *&ptr, const wxChar *spaceChars = wxT("\t "))
{
	while (*ptr && !IsSpaceChar(*ptr))
		ptr++;
}


pgHbaConfigLine::pgHbaConfigLine(const wxString &line)
{
	item = -1;
	isValid = false;
	isComment = true;
	connectType = PGC_INVALIDCONF;
	method = PGC_INVALIDMETHOD;
	Init(line);
}


void pgHbaConfigLine::Init(const wxString &line)
{
	isValid = false;
	connectType = PGC_INVALIDCONF;
	changed = false;

	if (line.IsEmpty())
		return;

	text = line;

	const wxChar *p0 = line.c_str();

	if (*p0 == '#')
	{
		isComment = true;
		p0++;
		SkipSpace(p0);
	}
	else
		isComment = false;


	const wxChar *p1 = p0;
	SkipNonspace(p1);

	wxString str = line.Mid(p0 - line.c_str(), p1 - p0);

	int i = FindToken(str, pgHbaConnectTypeStrings);

	if (i >= 0)
		connectType = (pgHbaConnectType)i;
	else
	{
		connectType = PGC_INVALIDCONF;
		return;
	}

	SkipSpace(p1);

	const wxChar *p2 = p1;
	bool quoted = false;

	while (*p2)
	{
		if (!quoted && IsSpaceChar(*p2))
			break;
		if (*p2 == '"')
			quoted = !quoted;
		p2++;
	}

	database = line.Mid(p1 - line.c_str(), p2 - p1);

	SkipSpace(p2);

	const wxChar *p3 = p2;

	quoted = false;
	while (*p3)
	{
		if (!quoted && IsSpaceChar(*p3))
			break;
		if (*p3 == '"')
			quoted = !quoted;
		p3++;
	}

	user = line.Mid(p2 - line.c_str(), p3 - p2);

	SkipSpace(p3);

	const wxChar *p4 = p3;

	if (connectType == PGC_LOCAL)
	{
		// no ip address
	}
	else
	{
		bool hasCidr = false;
		while (*p4 && !IsSpaceChar(*p4))
		{
			if (*p4 == '/')
				hasCidr = true;
			p4++;
		}
		if (!hasCidr)
		{
			SkipSpace(p4);
			SkipNonspace(p4);
		}

		ipaddress = line.Mid(p3 - line.c_str(), p4 - p3);
		SkipSpace(p4);
	}

	const wxChar *p5 = p4;
	SkipNonspace(p5);

	str = line.Mid(p4 - line.c_str(), p5 - p4);

	i = FindToken(str, pgHbaMethodStrings);

	if (i >= 0)
		method = (pgHbaMethod)i;
	else
	{
		return;
	}
	SkipSpace(p5);
	option = p5;

	isValid = true;
}


const wxChar *pgHbaConfigLine::GetConnectType()
{
	if (connectType >= 0 && connectType < PGC_INVALIDCONF)
		return pgHbaConnectTypeStrings[connectType];
	return 0;
}


const wxChar *pgHbaConfigLine::GetMethod()
{
	if (method >= 0 && method < PGC_INVALIDMETHOD)
		return pgHbaMethodStrings[method];
	return 0;
}


wxString pgHbaConfigLine::GetText()
{
	if (!changed)
		return text;

	wxString str;
	wxString tabspace = wxT("\t ");
	if (isComment)
		str = wxT("# ");

	str += GetConnectType()
	       +  tabspace + database
	       +  tabspace + user;

	if (connectType != PGC_LOCAL)
		str += tabspace + ipaddress;

	str += tabspace + GetMethod();

	if (method >= PGC_IDENT && !option.IsEmpty())
		str += tabspace + option;

	return str;
}


////////////////////////////////////////////////
pgPassConfigLine::pgPassConfigLine(const wxString &line)
{
	item = -1;
	Init(line);
}

void pgPassConfigLine::Init(const wxString &line)
{
	text = line;

	if (line.IsEmpty())
		return;

	isComment = line.StartsWith(wxT("#"));

	wxString tmpLine =  line;
	// De-escape backslash "\"
	tmpLine.Replace(wxT("\\\\"), wxT("\\"));

	int iVarCount = 0;
	wxStringTokenizer tok(isComment ? tmpLine.Mid(1) : tmpLine, wxT(":"));
	while(tok.HasMoreTokens())
	{
		wxString val = tok.GetNextToken();
		if (!val.IsEmpty())
		{
			/* if last charecter of the token is backslash "\" then it means
			   that it is used to escape ":" so we need to add the next token.
			*/
			while(val.Last() == wxT('\\'))
			{
				val.RemoveLast();
				val = val +  wxT(":") + tok.GetNextToken();
			}

			iVarCount++;
			switch(iVarCount)
			{
				case 1: // hostname
					hostname = val;
					break;
				case 2: // port
					port = val;
					break;
				case 3: // database
					database = val;
					break;
				case 4: //username
					username = val;
					break;
				case 5: // password
					password = val;
					break;
			}
		}
	}
}

wxString pgPassConfigLine::GetText()
{
	return (isComment ? wxT("#") : wxT("")) +
	       hostname + wxT(":") +
	       port + wxT(":") +
	       database + wxT(":") +
	       username + wxT(":") +
	       password;
}
