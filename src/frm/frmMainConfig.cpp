//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2005, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// frmMainConfig.cpp - Backend configuration tool
//
//////////////////////////////////////////////////////////////////////////

#include "pgAdmin3.h"

#ifdef __WXMSW__
#include <io.h>
#include <fcntl.h>
#endif

#include <wx/imaglist.h>

#include "frmMainConfig.h"
#include "frmMain.h"
#include "dlgMainConfig.h"
#include "utffile.h"
#include "pgConn.h"
#include "pgSet.h"
#include "menu.h"
#include "pgfeatures.h"

#include <wx/arrimpl.cpp>
WX_DEFINE_OBJARRAY(pgConfigOrgLineArray);

#define CTL_CFGVIEW 345

#include "images/pgAdmin3.xpm"
#include "images/elephant32.xpm"


BEGIN_EVENT_TABLE(frmMainConfig, frmConfig)
    EVT_MENU(MNU_UNDO,                      frmMainConfig::OnUndo)
    EVT_MENU(MNU_CONTENTS,                  frmMainConfig::OnContents)
    EVT_LIST_ITEM_ACTIVATED(CTL_CFGVIEW,    frmMainConfig::OnEditSetting)
END_EVENT_TABLE()


#define BCE_TITLE wxString(wxT("pgAdmin III - ")) + _("Backend Configuration Editor")


frmMainConfig::frmMainConfig(frmMain *parent, pgServer *server)
: frmConfig(parent, BCE_TITLE, 0)
{
    if (server)
        conn = server->CreateConn();

    Init();

    if (conn)
    {

        if (serverFileName.IsEmpty())
            serverFileName = wxT("postgresql.conf");


        wxString txt;
        txt.Printf(_(" - %s on %s (%s:%d)"),
                serverFileName.c_str(), server->GetDescription().c_str(), 
                server->GetName().c_str(), server->GetPort());
        SetTitle(BCE_TITLE + txt);

        wxString str;
        str = conn->ExecuteScalar(wxT("SELECT pg_file_read('") + serverFileName + wxT("', 0, ")
                    wxT("pg_file_length('") + serverFileName + wxT("'))"));

        DisplayFile(str);

        statusBar->SetStatusText(wxString::Format(_(" Configuration read from %s"), conn->GetHost().c_str()));
    }
}


frmMainConfig::frmMainConfig(const wxString& title, const wxString &configFile)
: frmConfig(title + wxT(" - ") + _("Backend Configuration Editor"), configFile)
{
    Init();
    OpenLastFile();
}


frmMainConfig::~frmMainConfig()
{
    options.clear();
    lines.Empty();
}


void frmMainConfig::Init(pgSettingReader *reader)
{
    pgSettingItem *item;
    do
    {
        item = reader->GetNextItem();
        if (item)
        {
            if (item->context == pgSettingItem::PGC_INTERNAL)
                delete item;
            else
            {
                if (options[item->name])
                    delete item;
                else
                {
                    options[item->name] = item;

                    wxArrayString *category = categories[item->category];
                    if (!category)
                    {
                        category = new wxArrayString;
                        categories[item->category] = category;
                    }
                    category->Add(item->name);
                }
            }
        }
    }
    while (item);
}


void frmMainConfig::Init()
{
    wxIconBundle icons;
    icons.AddIcon(wxIcon(pgAdmin3_xpm));
    icons.AddIcon(wxIcon(elephant32_xpm));
    SetIcons(icons);
 
    InitFrame(wxT("frmMainConfig"));
    RestorePosition(50, 50, 600, 600, 300, 200);

    cfgList = new ctlListView(this, CTL_CFGVIEW, wxDefaultPosition, wxDefaultSize, wxSIMPLE_BORDER);

    cfgList->SetImageList(configImageList, wxIMAGE_LIST_SMALL);

    cfgList->AddColumn(_("Setting name"), 120);
    cfgList->AddColumn(_("Value"), 80);
    if (conn)
        cfgList->AddColumn(_("Current value"), 80);
    cfgList->AddColumn(_("Comment"), 400);


    pgSettingReader *reader;
    if (conn)
    {
        // read settings from server
        reader = new pgSettingDbReader(conn);
    }
    else
    {
        // read settings from file. First, use localized file...
        reader = new pgSettingFileReader(true);

        if (reader->IsValid())
            Init(reader);
        delete reader;

        // ... then add default file
        reader = new pgSettingFileReader(false);
    }

    if (reader->IsValid())
        Init(reader);

    delete reader;
}


void frmMainConfig::OnEditSetting(wxListEvent& event)
{
    wxString name=cfgList->GetText(event.GetIndex());
    if (!name.IsEmpty())
    {
        pgSettingItem *item = options[name];
        wxASSERT(item);
        dlgMainConfig dlg(this, item);
        dlg.Go();

        if (item->orgLine && !item->newLine->Differs(item->orgLine))
        {
            delete item->newLine;
            item->newLine = 0;
        }
        else
        {
            changed=true;
            fileMenu->Enable(MNU_SAVE, true);
            editMenu->Enable(MNU_UNDO, true);
            toolBar->EnableTool(MNU_SAVE, true);
            toolBar->EnableTool(MNU_UNDO, true);
        }
        UpdateLine(event.GetIndex());
    }
}


void frmMainConfig::OnContents(wxCommandEvent& event)
{
    DisplayHelp(this, wxT("config"));
}


wxString frmMainConfig::GetHelpPage() const
{
    wxString page;
    if (page.IsEmpty())
    	page=wxT("pg/runtime-config");

    return page;
}


void frmMainConfig::OnUndo(wxCommandEvent& ev)
{
    wxString name=cfgList->GetText(cfgList->GetSelection());
    if (!name.IsEmpty())
    {
        pgSettingItem *item = options[name];
        wxASSERT(name);
        if (item->newLine)
        {
            delete item->newLine;
            item->newLine = 0;
            UpdateLine(cfgList->GetSelection());
        }
    }
}



void frmMainConfig::UpdateLine(int pos)
{
    wxString name=cfgList->GetText(pos);
    if (!name.IsEmpty())
    {
        pgSettingItem *item = options[name];
        wxASSERT(name);

        pgConfigLine *line=item->newLine;
        if (!line)
            line = item->orgLine;

        wxString value, comment;
        int imgIndex=0;
        if (line)
        {
            value = line->value;
            comment = line->comment;
            if (!line->isComment)
                imgIndex = 1;
        }
        cfgList->SetItem(pos, 1, value);

        if (conn)
            cfgList->SetItem(pos, 3, comment);
        else
            cfgList->SetItem(pos, 2, comment);

        cfgList->SetItemImage(pos, imgIndex, imgIndex);
    }
}


void frmMainConfig::WriteFile(pgConn *conn)
{

    size_t i;

    wxString str;
    for (i=0 ; i < lines.GetCount() ; i++)
        str.Append(lines.Item(i).GetNewText() + wxT("\n"));

    for (i=0 ; i < (size_t)cfgList->GetItemCount() ; i++)
    {
        pgSettingItem *item = options[cfgList->GetText(i)];

        if (item && item->newLine && !item->orgLine)
            str.Append(item->newLine->GetNewText() + wxT("\n"));
    }


    if (DoWriteFile(str, conn))
    {
        changed=false;
        fileMenu->Enable(MNU_SAVE, false);
        editMenu->Enable(MNU_UNDO, false);
        toolBar->EnableTool(MNU_SAVE, false);
        toolBar->EnableTool(MNU_UNDO, false);

        size_t i;
        for (i=0 ; i < (size_t)cfgList->GetItemCount() ; i++)
        {
            pgSettingItem *item = options[cfgList->GetText(i)];

            if (item && item->newLine)
            {
                if (!item->orgLine)
                {
                    item->orgLine = new pgConfigOrgLine(item->newLine);
                    lines.Add(item->orgLine);
                    item->orgLine->item = item;
                }
                else
                {
                    item->orgLine->comment = item->newLine->comment;
                    item->orgLine->isComment = item->newLine->isComment;
                    item->orgLine->value = item->newLine->value;
                    item->orgLine->text = item->orgLine->GetNewText();
                }
            }
        }


        for (i=0 ; i < lines.GetCount() ; i++)
        {
            pgConfigOrgLine &line = lines.Item(i);
            if (line.item && line.item->newLine)
            {
                line.text = line.GetNewText();
                delete line.item->newLine;
                line.item->newLine = 0;
            }
        }
    }
}



void frmMainConfig::DisplayFile(const wxString &str)
{
    lines.Empty();

    filetype = wxTextFileType_Unix;
    wxStringTokenizer strtok;
    wxArrayString *unknownCategory=0;

    if (str.Find('\r') >= 0)
    {
        if (str.Find(wxT("\n\r")) >= 0 || str.Find(wxT("\r\n")))
            filetype = wxTextFileType_Dos;
        else
            filetype = wxTextFileType_Mac;

        strtok.SetString(wxTextBuffer::Translate(str, wxTextFileType_Unix), wxT("\n"), wxTOKEN_RET_EMPTY);
    }
    else
        strtok.SetString(str, wxT("\n"), wxTOKEN_RET_EMPTY);

    while (strtok.HasMoreTokens())
    {
        pgConfigOrgLine *line=new pgConfigOrgLine(strtok.GetNextToken());
        lines.Add(line);

        // identify option
        bool isComment=false;
        const wxChar *p=line->text.c_str();

        // identify keywords
        while (*p && wxStrchr(wxT("# \n"), *p))
        {
            if (*p == '#')
                isComment=true;
            p++;
        }

        if (!*p)
            isComment = true;

        line->isComment = isComment;

        const wxChar *p2=p;
        while (*p2 && *p2 != '#' && *p2 != ' ' && *p2 != '\t' && *p2 != '=')
            p2++;

        if (p2 != p)
        {
            wxString keyword=line->text.Mid(p-line->text.c_str(), p2-p);

            pgSettingItemHashmap::iterator it = options.find(keyword);

            pgSettingItem *item;
            if (it == options.end())
            {
                if (isComment)
                    continue;

                item = new pgSettingItem;
                item->name = keyword;
                item->category = _("Unknown");
                item->short_desc = _("Unknown option");
                item->extra_desc = _("This option is present in the configuration file, but not known to the configuration tool.");
                item->SetType(wxT("string"));

                options[item->name] = item;

                if (!unknownCategory)
                {
                    unknownCategory = new wxArrayString;
                    categories[item->category] = unknownCategory;
                }
                unknownCategory->Add(item->name);
            }
            else
                item = it->second;


            if (!isComment || !item->orgLine || item->orgLine->isComment)
            {
                line->item = item;
                if (item->orgLine)
                    item->orgLine->item = 0;
                item->orgLine = line;
            }
            while (*p2 && *p2 != '=')
                p2++;

            p2++;   // skip =
            while (*p2 && wxStrchr(wxT(" \t"), *p2))
                p2++;

            wxChar quoteChar=0;
            if (wxStrchr(wxT("'\""), *p2))
                quoteChar = *p2++;

            const wxChar *p3=p2;
            while (*p3)
            {
                if (*p3 == quoteChar || (!quoteChar && wxStrchr(wxT(" \t#"), *p3)))
                    break;
                p3++;
            }
            if (p2 != p3)
            {
                line->value = line->text.Mid(p2-line->text.c_str(), p3-p2);
                if (quoteChar)
                    p3++;

                const wxChar *p4=p3;
                while (*p4 && wxStrchr(wxT(" \t#"), *p4))
                    p4++;

                line->commentIndent = line->text.Mid(p3-line->text.c_str(), p4-p3);
                line->comment = p4;
            }
        }
    }

    cfgList->DeleteAllItems();

    // we want to show options ordered by category/name
    // category might be localized, and we want a distinct category ordering 

    FillList(wxT("listen_addresses"));          // Connections and Authentication / Connection Settings
    FillList(wxT("authentication_timeout"));    // Connections and Authentication / Security and Authentication
    FillList(wxT("check_function_bodies"));     // Client Connection Defaults / Statement Behaviour
    FillList(wxT("lc_messages"));               // Client Connection Defaults / Locale and Formatting
    FillList(wxT("explain_pretty_print"));      // Client Connection Defaults / Other Defaults
    FillList(wxT("enable_hashjoin"));           // Query Tuning / Planner Method Configuration
    FillList(wxT("cpu_operator_cost"));         // Query Tuning / Planner Cost Constants
    FillList(wxT("geqo"));                      // Query Tuning / Genetic Query Optimizer
    FillList(wxT("default_statistics_target")); // Query Tuning / Other Planner Options
    FillList(wxT("deadlock_timeout"));          // Lock Management
    FillList(wxT("shared_buffers"));            // Resource Usage / Memory
    FillList(wxT("max_fsm_pages"));             // Resource Usage / Free Space Map
    FillList(wxT("bgwriter_delay"));            // Resource Usage
    FillList(wxT("max_files_per_process"));     // Resource Usage / Kernel Resources
    FillList(wxT("log_connections"));           // Reporting and Logging / What to Log
    FillList(wxT("client_min_messages"));       // Reporting and Logging / When to Log
    FillList(wxT("log_destination"));           // Reporting and Logging / Where to Log
    FillList(wxT("stats_command_string"));      // Statistics / Query and Index Statistics Collector
    FillList(wxT("log_executor_stats"));        // Statistics / Monitoring
    FillList(wxT("fsync"));                     // Write-Ahead Log / Settings
    FillList(wxT("checkpoint_segments"));       // Write-Ahead Log / Checkpoints
    FillList(wxT("add_missing_from"));          // Version and Platform Compatibility / Previous PostgreSQL Version
    FillList(wxT("transform_null_equals"));     // Version and Platform Compatibility / Other Platforms and Clients
    FillList(wxT("trace_notify"));              // Developer Options
    FillList(wxT("hba_file"));                  // Ungrouped


    // for all we didn't get 
    while (!categories.empty())
    {
        pgCategoryHashmap::iterator it = categories.begin();
        wxString missed=it->first;
        FillList(it->second);
        categories.erase(it);
    }
}


void frmMainConfig::FillList(const wxString &categoryMember)
{
    pgSettingItem *categoryItem = options[categoryMember];
    wxASSERT_MSG(categoryItem, categoryMember.c_str());

    if (categoryItem)
    {
        FillList(categories[categoryItem->category]);
        categories.erase(categoryItem->category);
    }
}


void frmMainConfig::FillList(wxArrayString *category)
{
    if (category)
    {
        size_t i;
        for (i=0 ; i < category->GetCount() ; i++)
        {
            pgSettingItem *item = options[category->Item(i)];
            wxASSERT(item);

            wxString value;
            wxString comment;
            int imgIndex=0;
            if (item->orgLine)
            {
                if (!item->orgLine->isComment)
                    imgIndex=1;
                value = item->orgLine->value;
                comment = item->orgLine->comment;
                comment.Replace(wxT("\t"), wxT(" "));
            }
            long pos=cfgList->AppendItem(imgIndex, item->name, value);
            if (conn)
            {
                cfgList->SetItem(pos, 2, item->value);
                cfgList->SetItem(pos, 3, comment);
            }
            else
                cfgList->SetItem(pos, 2, comment);
        }
        delete category;
    }
}



enum
{
    HINT_LISTEN_ADDRESSES
};


const wxChar *hintString[]=
{
    _("The PostgreSQL server engine is currently configured to listen for local connections only.\nYou might want to check \"listen_addresses\" to enable accessing the server over the network too.")
};


wxString frmMainConfig::GetHintString()
{
    wxArrayInt hints;
    size_t i;
    for (i=0 ; i < (size_t)cfgList->GetItemCount() ; i++)
    {
        pgSettingItem *item = options[cfgList->GetText(i)];

        if (item)
        {
            wxString name = item->name;
            wxString value = item->GetActiveValue();
            if (name == wxT("listen_addresses"))
            {
                if (value.IsEmpty() || value == wxT("localhost"))
                    hints.Add(HINT_LISTEN_ADDRESSES);
            }
        }
    }

    wxString str;
    for (i=0 ; i < hints.GetCount() ; i++)
    {
        if (i)
            str.Append(wxT("\n\n"));
        str.Append(hintString[hints.Item(i)]);
    }

    return str;
}

