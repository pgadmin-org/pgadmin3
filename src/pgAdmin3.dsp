# Microsoft Developer Studio Project File - Name="pgAdmin3" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=pgAdmin3 - Win32 Release
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "pgAdmin3.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "pgAdmin3.mak" CFG="pgAdmin3 - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "pgAdmin3 - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE "pgAdmin3 - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "pgAdmin3 - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "pgAdmin3___Win32_Debug"
# PROP BASE Intermediate_Dir "pgAdmin3___Win32_Debug"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "c:/wxWidgets-2.6/include" /I "c:/wxWidgets-2.6/contrib/include" /I "c:/program files/postgresql/8.1/include" /I "include/" /I "agent/include" /I "slony/include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "__WINDOWS__" /D "__WXMSW__" /D DEBUG=1 /D "__WXDEBUG__" /D "__WIN95__" /D "__WIN32__" /D WINVER=0x0400 /D "STRICT" /D wxUSE_UNICODE=1 /D "_UNICODE" /D "UNICODE" /D "SSL" /FAcs /Fr /Yu"pgadmin3.h" /FD /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "c:/wxWidgets-2.6/include" /I "c:/wxWidgets-2.6/contrib/include" /I "c:/program files/postgresql/8.1/include" /I "include/" /I "agent/include" /I "slony/include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "__WINDOWS__" /D "__WXMSW__" /D DEBUG=1 /D "__WXDEBUG__" /D "__WIN95__" /D "__WIN32__" /D WINVER=0x0400 /D "STRICT" /D wxUSE_UNICODE=1 /D "_UNICODE" /D "UNICODE" /D "SSL" /FAcs /Fr /Yu"pgadmin3.h" /FD /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x809 /i "c:/wxWidgets-2.6/include" /d "_DEBUG"
# ADD RSC /l 0x809 /i "c:/wxWidgets-2.6/include" /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 libpq.lib wxbase26ud.lib wxbase26ud_xml.lib wxbase26ud_net.lib wxmsw26ud_adv.lib wxmsw26ud_core.lib wxmsw26ud_html.lib wxregexud.lib wxpngd.lib wxzlibd.lib wxjpegd.lib wxtiffd.lib wxmsw26ud_stc.lib wxmsw26ud_ogl.lib wxmsw26ud_xrc.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib uuid.lib comctl32.lib rpcrt4.lib wsock32.lib winmm.lib ole32.lib oleaut32.lib /nologo /subsystem:windows /map /debug /machine:I386 /nodefaultlib:"libcd.lib" /nodefaultlib:"libcid.lib" /nodefaultlib:"msvcrt.lib" /pdbtype:sept /libpath:"c:/wxWidgets-2.6/lib/vc_lib" /libpath:"c:/program files/postgresql/8.1/lib/ms"
# SUBTRACT BASE LINK32 /pdb:none
# ADD LINK32 libpq.lib wxbase26ud.lib wxbase26ud_xml.lib wxbase26ud_net.lib wxmsw26ud_adv.lib wxmsw26ud_core.lib wxmsw26ud_html.lib wxregexud.lib wxpngd.lib wxzlibd.lib wxjpegd.lib wxtiffd.lib wxmsw26ud_stc.lib wxmsw26ud_ogl.lib wxmsw26ud_xrc.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib uuid.lib comctl32.lib rpcrt4.lib wsock32.lib winmm.lib ole32.lib oleaut32.lib /nologo /subsystem:windows /map /debug /machine:I386 /nodefaultlib:"libcd.lib" /nodefaultlib:"libcid.lib" /nodefaultlib:"msvcrt.lib" /pdbtype:sept /libpath:"c:/wxWidgets-2.6/lib/vc_lib" /libpath:"c:/program files/postgresql/8.1/lib/ms"
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "pgAdmin3 - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "pgAdmin3___Win32_Release"
# PROP BASE Intermediate_Dir "pgAdmin3___Win32_Release"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /GX /O2 /Ob2 /I "c:/wxWidgets-2.6/include" /I "c:/wxWidgets-2.6/contrib/include" /I "c:/program files/postgresql/8.1/include" /I "include/" /I "agent/include" /I "slony/include" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "__WINDOWS__" /D "__WIN95__" /D "__WIN32__" /D WINVER=0x0400 /D "STRICT" /D wxUSE_UNICODE=1 /D "_UNICODE" /D "UNICODE" /D "EMBED_XRC" /D "SSL" /FAcs /FR /Yu"pgadmin3.h" /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /Ob2 /I "c:/wxWidgets-2.6/include" /I "c:/wxWidgets-2.6/contrib/include" /I "c:/program files/postgresql/8.1/include" /I "include/" /I "agent/include" /I "slony/include" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "__WINDOWS__" /D "__WIN95__" /D "__WIN32__" /D WINVER=0x0400 /D "STRICT" /D wxUSE_UNICODE=1 /D "_UNICODE" /D "UNICODE" /D "EMBED_XRC" /D "SSL" /FAcs /FR /Yu"pgadmin3.h" /FD /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x809 /i "c:/wxWidgets-2.6/include" /d "NDEBUG"
# ADD RSC /l 0x809 /i "c:/wxWidgets-2.6/include" /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 libpq.lib wxbase26u.lib wxbase26u_xml.lib wxbase26u_net.lib wxmsw26u_adv.lib wxmsw26u_core.lib wxmsw26u_html.lib wxregexu.lib wxpng.lib wxzlib.lib wxjpeg.lib wxtiff.lib wxmsw26u_stc.lib wxmsw26u_ogl.lib wxmsw26u_xrc.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib uuid.lib comctl32.lib rpcrt4.lib wsock32.lib winmm.lib ole32.lib oleaut32.lib /nologo /subsystem:windows /map /machine:I386 /nodefaultlib:"libcd.lib" /nodefaultlib:"libcid.lib" /nodefaultlib:"msvcrtd.lib" /pdbtype:sept /libpath:"c:/wxWidgets-2.6/lib/vc_lib" /libpath:"c:/program files/postgresql/8.1/lib/ms"
# SUBTRACT BASE LINK32 /pdb:none
# ADD LINK32 libpq.lib wxbase26u.lib wxbase26u_xml.lib wxbase26u_net.lib wxmsw26u_adv.lib wxmsw26u_core.lib wxmsw26u_html.lib wxregexu.lib wxpng.lib wxzlib.lib wxjpeg.lib wxtiff.lib wxmsw26u_stc.lib wxmsw26u_ogl.lib wxmsw26u_xrc.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib uuid.lib comctl32.lib rpcrt4.lib wsock32.lib winmm.lib ole32.lib oleaut32.lib /nologo /subsystem:windows /map /machine:I386 /nodefaultlib:"libcd.lib" /nodefaultlib:"libcid.lib" /nodefaultlib:"msvcrtd.lib" /pdbtype:sept /libpath:"c:/wxWidgets-2.6/lib/vc_lib" /libpath:"c:/program files/postgresql/8.1/lib/ms"
# SUBTRACT LINK32 /pdb:none

!ENDIF 

# Begin Target

# Name "pgAdmin3 - Win32 Debug"
# Name "pgAdmin3 - Win32 Release"
# Begin Group "src"

# PROP Default_Filter ""
# Begin Group "db"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\db\keywords.c
# SUBTRACT BASE CPP /YX /Yc /Yu
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\db\pgConn.cpp
# End Source File
# Begin Source File

SOURCE=.\db\pgSet.cpp
# End Source File
# End Group
# Begin Group "schema"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\schema\pgAggregate.cpp
# End Source File
# Begin Source File

SOURCE=.\schema\pgCast.cpp
# End Source File
# Begin Source File

SOURCE=.\schema\pgCheck.cpp
# End Source File
# Begin Source File

SOURCE=.\schema\pgCollection.cpp
# End Source File
# Begin Source File

SOURCE=.\schema\pgColumn.cpp
# End Source File
# Begin Source File

SOURCE=.\schema\pgConstraints.cpp
# End Source File
# Begin Source File

SOURCE=.\schema\pgConversion.cpp
# End Source File
# Begin Source File

SOURCE=.\schema\pgDatabase.cpp
# End Source File
# Begin Source File

SOURCE=.\schema\pgDatatype.cpp
# End Source File
# Begin Source File

SOURCE=.\schema\pgDomain.cpp
# End Source File
# Begin Source File

SOURCE=.\schema\pgForeignKey.cpp
# End Source File
# Begin Source File

SOURCE=.\schema\pgFunction.cpp
# End Source File
# Begin Source File

SOURCE=.\schema\pgGroup.cpp
# End Source File
# Begin Source File

SOURCE=.\schema\pgIndex.cpp
# End Source File
# Begin Source File

SOURCE=.\schema\pgIndexConstraint.cpp
# End Source File
# Begin Source File

SOURCE=.\schema\pgLanguage.cpp
# End Source File
# Begin Source File

SOURCE=.\schema\pgObject.cpp
# End Source File
# Begin Source File

SOURCE=.\schema\pgOperator.cpp
# End Source File
# Begin Source File

SOURCE=.\schema\pgOperatorClass.cpp
# End Source File
# Begin Source File

SOURCE=.\schema\pgRole.cpp
# End Source File
# Begin Source File

SOURCE=.\schema\pgRule.cpp
# End Source File
# Begin Source File

SOURCE=.\schema\pgSchema.cpp
# End Source File
# Begin Source File

SOURCE=.\schema\pgSequence.cpp
# End Source File
# Begin Source File

SOURCE=.\schema\pgServer.cpp
# End Source File
# Begin Source File

SOURCE=.\schema\pgTable.cpp
# End Source File
# Begin Source File

SOURCE=.\schema\pgTablespace.cpp
# End Source File
# Begin Source File

SOURCE=.\schema\pgTrigger.cpp
# End Source File
# Begin Source File

SOURCE=.\schema\pgType.cpp
# End Source File
# Begin Source File

SOURCE=.\schema\pgUser.cpp
# End Source File
# Begin Source File

SOURCE=.\schema\pgView.cpp
# End Source File
# End Group
# Begin Group "utils"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\utils\misc.cpp
# End Source File
# Begin Source File

SOURCE=.\utils\pgconfig.cpp
# End Source File
# Begin Source File

SOURCE=.\utils\precomp.cpp
# ADD BASE CPP /Yc"pgAdmin3.h"
# ADD CPP /Yc"pgAdmin3.h"
# End Source File
# Begin Source File

SOURCE=.\utils\sysProcess.cpp
# End Source File
# Begin Source File

SOURCE=.\utils\sysSettings.cpp
# End Source File
# Begin Source File

SOURCE=.\utils\tabcomplete.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\utils\update.cpp
# End Source File
# Begin Source File

SOURCE=.\utils\utffile.cpp
# SUBTRACT BASE CPP /YX /Yc /Yu
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# End Group
# Begin Group "ui"

# PROP Default_Filter ""
# Begin Group "Properties"

# PROP Default_Filter "*.cpp"
# Begin Source File

SOURCE=.\dlg\dlgAggregate.cpp
# End Source File
# Begin Source File

SOURCE=.\dlg\dlgCast.cpp
# End Source File
# Begin Source File

SOURCE=.\dlg\dlgCheck.cpp
# End Source File
# Begin Source File

SOURCE=.\dlg\dlgColumn.cpp
# End Source File
# Begin Source File

SOURCE=.\dlg\dlgConversion.cpp
# End Source File
# Begin Source File

SOURCE=.\dlg\dlgDatabase.cpp
# End Source File
# Begin Source File

SOURCE=.\dlg\dlgDomain.cpp
# End Source File
# Begin Source File

SOURCE=.\dlg\dlgForeignKey.cpp
# End Source File
# Begin Source File

SOURCE=.\dlg\dlgFunction.cpp
# End Source File
# Begin Source File

SOURCE=.\dlg\dlgGroup.cpp
# End Source File
# Begin Source File

SOURCE=.\dlg\dlgIndex.cpp
# End Source File
# Begin Source File

SOURCE=.\dlg\dlgIndexConstraint.cpp
# End Source File
# Begin Source File

SOURCE=.\dlg\dlgLanguage.cpp
# End Source File
# Begin Source File

SOURCE=.\dlg\dlgOperator.cpp
# End Source File
# Begin Source File

SOURCE=.\dlg\dlgProperty.cpp
# End Source File
# Begin Source File

SOURCE=.\dlg\dlgRole.cpp
# End Source File
# Begin Source File

SOURCE=.\dlg\dlgRule.cpp
# End Source File
# Begin Source File

SOURCE=.\dlg\dlgSchema.cpp
# End Source File
# Begin Source File

SOURCE=.\dlg\dlgSequence.cpp
# End Source File
# Begin Source File

SOURCE=.\dlg\dlgServer.cpp
# End Source File
# Begin Source File

SOURCE=.\dlg\dlgTable.cpp
# End Source File
# Begin Source File

SOURCE=.\dlg\dlgTablespace.cpp
# End Source File
# Begin Source File

SOURCE=.\dlg\dlgTrigger.cpp
# End Source File
# Begin Source File

SOURCE=.\dlg\dlgType.cpp
# End Source File
# Begin Source File

SOURCE=.\dlg\dlgUser.cpp
# End Source File
# Begin Source File

SOURCE=.\dlg\dlgView.cpp
# End Source File
# End Group
# Begin Group "xrc dialogs"

# PROP Default_Filter ".xrc"
# Begin Source File

SOURCE=.\ui\dlgAggregate.xrc
# End Source File
# Begin Source File

SOURCE=.\ui\dlgCast.xrc
# End Source File
# Begin Source File

SOURCE=.\ui\dlgCheck.xrc
# End Source File
# Begin Source File

SOURCE=.\ui\dlgColumn.xrc
# End Source File
# Begin Source File

SOURCE=.\ui\dlgConversion.xrc
# End Source File
# Begin Source File

SOURCE=.\ui\dlgDatabase.xrc
# End Source File
# Begin Source File

SOURCE=.\ui\dlgDomain.xrc
# End Source File
# Begin Source File

SOURCE=.\ui\dlgEditGridOptions.xrc
# End Source File
# Begin Source File

SOURCE=.\ui\dlgForeignKey.xrc
# End Source File
# Begin Source File

SOURCE=.\ui\dlgFunction.xrc
# End Source File
# Begin Source File

SOURCE=.\ui\dlgGroup.xrc
# End Source File
# Begin Source File

SOURCE=.\ui\dlgHbaConfig.xrc
# End Source File
# Begin Source File

SOURCE=.\ui\dlgIndex.xrc
# End Source File
# Begin Source File

SOURCE=.\ui\dlgIndexConstraint.xrc
# End Source File
# Begin Source File

SOURCE=.\ui\dlgJob.xrc
# End Source File
# Begin Source File

SOURCE=.\ui\dlgLanguage.xrc
# End Source File
# Begin Source File

SOURCE=.\ui\dlgMainConfig.xrc
# End Source File
# Begin Source File

SOURCE=.\ui\dlgOperator.xrc
# End Source File
# Begin Source File

SOURCE=.\ui\dlgRule.xrc
# End Source File
# Begin Source File

SOURCE=.\ui\dlgSchedule.xrc
# End Source File
# Begin Source File

SOURCE=.\ui\dlgSchema.xrc
# End Source File
# Begin Source File

SOURCE=.\ui\dlgSequence.xrc
# End Source File
# Begin Source File

SOURCE=.\ui\dlgServer.xrc
# End Source File
# Begin Source File

SOURCE=.\ui\dlgStep.xrc
# End Source File
# Begin Source File

SOURCE=.\ui\dlgTable.xrc
# End Source File
# Begin Source File

SOURCE=.\ui\dlgTrigger.xrc
# End Source File
# Begin Source File

SOURCE=.\ui\dlgType.xrc
# End Source File
# Begin Source File

SOURCE=.\ui\dlgUser.xrc
# End Source File
# Begin Source File

SOURCE=.\ui\dlgView.xrc
# End Source File
# Begin Source File

SOURCE=.\ui\frmAddTableView.xrc
# End Source File
# Begin Source File

SOURCE=.\ui\frmConnect.xrc
# End Source File
# Begin Source File

SOURCE=.\ui\frmExport.xrc
# End Source File
# Begin Source File

SOURCE=.\ui\frmOptions.xrc
# End Source File
# Begin Source File

SOURCE=.\ui\frmPassword.xrc
# End Source File
# Begin Source File

SOURCE=.\ui\frmServer.xrc
# End Source File
# Begin Source File

SOURCE=.\ui\frmStatus.xrc
# End Source File
# Begin Source File

SOURCE=.\ui\frmVacuum.xrc
# End Source File
# Begin Source File

SOURCE=.\ui\xrcDialogs.cpp
# SUBTRACT BASE CPP /YX /Yc /Yu
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# End Group
# Begin Group "Forms"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\frm\frmAbout.cpp
# End Source File
# Begin Source File

SOURCE=.\frm\frmBackup.cpp
# End Source File
# Begin Source File

SOURCE=.\frm\frmConfig.cpp
# End Source File
# Begin Source File

SOURCE=.\frm\frmEditGrid.cpp
# End Source File
# Begin Source File

SOURCE=.\frm\frmExport.cpp
# End Source File
# Begin Source File

SOURCE=.\frm\frmGrantWizard.cpp
# End Source File
# Begin Source File

SOURCE=.\frm\frmHbaConfig.cpp
# End Source File
# Begin Source File

SOURCE=.\frm\frmHelp.cpp
# End Source File
# Begin Source File

SOURCE=.\frm\frmHint.cpp
# End Source File
# Begin Source File

SOURCE=.\frm\frmIndexcheck.cpp
# End Source File
# Begin Source File

SOURCE=.\frm\frmMain.cpp
# End Source File
# Begin Source File

SOURCE=.\frm\frmMainConfig.cpp
# End Source File
# Begin Source File

SOURCE=.\frm\frmMaintenance.cpp
# End Source File
# Begin Source File

SOURCE=.\frm\frmOptions.cpp
# End Source File
# Begin Source File

SOURCE=.\frm\frmPassword.cpp
# End Source File
# Begin Source File

SOURCE=.\frm\frmPgpassConfig.cpp
# End Source File
# Begin Source File

SOURCE=.\frm\frmQuery.cpp
# End Source File
# Begin Source File

SOURCE=.\frm\frmRestore.cpp
# End Source File
# Begin Source File

SOURCE=.\frm\frmSplash.cpp
# End Source File
# Begin Source File

SOURCE=.\frm\frmStatus.cpp
# End Source File
# Begin Source File

SOURCE=.\frm\frmUpdate.cpp
# End Source File
# End Group
# Begin Group "Main"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\main\dlgClasses.cpp
# End Source File
# Begin Source File

SOURCE=.\main\events.cpp
# End Source File
# Begin Source File

SOURCE=.\pgAdmin3.cpp
# End Source File
# Begin Source File

SOURCE=.\main\pgAdmin3.rc
# End Source File
# End Group
# Begin Group "Dialogs"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\dlg\dlgConnect.cpp
# End Source File
# Begin Source File

SOURCE=.\dlg\dlgEditGridOptions.cpp
# End Source File
# Begin Source File

SOURCE=.\dlg\dlgHbaConfig.cpp
# End Source File
# Begin Source File

SOURCE=.\dlg\dlgMainConfig.cpp
# End Source File
# Begin Source File

SOURCE=.\dlg\dlgPgpassConfig.cpp
# End Source File
# Begin Source File

SOURCE=.\dlg\dlgSelectConnection.cpp
# End Source File
# End Group
# Begin Group "Controls"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\ctl\calbox.cpp
# SUBTRACT BASE CPP /YX /Yc /Yu
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\ctl\ctlComboBox.cpp
# SUBTRACT BASE CPP /YX /Yc /Yu
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\ctl\ctlListView.cpp
# SUBTRACT BASE CPP /YX /Yc /Yu
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\ctl\ctlSecurityPanel.cpp
# End Source File
# Begin Source File

SOURCE=.\ctl\ctlSQLBox.cpp
# End Source File
# Begin Source File

SOURCE=.\ctl\ctlSQLResult.cpp
# SUBTRACT BASE CPP /YX /Yc /Yu
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\ctl\ctlTree.cpp
# End Source File
# Begin Source File

SOURCE=.\ctl\explainCanvas.cpp
# End Source File
# Begin Source File

SOURCE=.\ctl\explainShape.cpp
# End Source File
# Begin Source File

SOURCE=.\ctl\timespin.cpp
# SUBTRACT BASE CPP /YX /Yc /Yu
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\ctl\xh_calb.cpp
# SUBTRACT BASE CPP /YX /Yc /Yu
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\ctl\xh_ctlcombo.cpp
# SUBTRACT BASE CPP /YX /Yc /Yu
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\ctl\xh_sqlbox.cpp
# SUBTRACT BASE CPP /YX /Yc /Yu
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\ctl\xh_timespin.cpp
# SUBTRACT BASE CPP /YX /Yc /Yu
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# End Group
# End Group
# Begin Group "include"

# PROP Default_Filter ""
# Begin Group "ui includes"

# PROP Default_Filter "*.h"
# Begin Source File

SOURCE=.\include\dlgConnect.h
# End Source File
# Begin Source File

SOURCE=.\include\dlgEditGridOptions.h
# End Source File
# Begin Source File

SOURCE=.\include\dlgHbaConfig.h
# End Source File
# Begin Source File

SOURCE=.\include\dlgMainConfig.h
# End Source File
# Begin Source File

SOURCE=.\include\dlgPgpassConfig.h
# End Source File
# Begin Source File

SOURCE=.\include\dlgSelectConnection.h
# End Source File
# Begin Source File

SOURCE=.\include\frmAbout.h
# End Source File
# Begin Source File

SOURCE=.\include\frmBackup.h
# End Source File
# Begin Source File

SOURCE=.\include\frmConfig.h
# End Source File
# Begin Source File

SOURCE=.\include\frmEditGrid.h
# End Source File
# Begin Source File

SOURCE=.\include\frmExport.h
# End Source File
# Begin Source File

SOURCE=.\include\frmGrantWizard.h
# End Source File
# Begin Source File

SOURCE=.\include\frmHbaConfig.h
# End Source File
# Begin Source File

SOURCE=.\include\frmHelp.h
# End Source File
# Begin Source File

SOURCE=.\include\frmHint.h
# End Source File
# Begin Source File

SOURCE=.\include\frmIndexcheck.h
# End Source File
# Begin Source File

SOURCE=.\include\frmMain.h
# End Source File
# Begin Source File

SOURCE=.\include\frmMainConfig.h
# End Source File
# Begin Source File

SOURCE=.\include\frmMaintenance.h
# End Source File
# Begin Source File

SOURCE=.\include\frmOptions.h
# End Source File
# Begin Source File

SOURCE=.\include\frmPassword.h
# End Source File
# Begin Source File

SOURCE=.\include\frmPgpassConfig.h
# End Source File
# Begin Source File

SOURCE=.\include\frmQuery.h
# End Source File
# Begin Source File

SOURCE=.\include\frmRestore.h
# End Source File
# Begin Source File

SOURCE=.\include\frmSplash.h
# End Source File
# Begin Source File

SOURCE=.\include\frmStatus.h
# End Source File
# Begin Source File

SOURCE=.\include\frmUpdate.h
# End Source File
# End Group
# Begin Group "schema includes"

# PROP Default_Filter "*.h"
# Begin Source File

SOURCE=.\include\pgAggregate.h
# End Source File
# Begin Source File

SOURCE=.\include\pgCast.h
# End Source File
# Begin Source File

SOURCE=.\include\pgCheck.h
# End Source File
# Begin Source File

SOURCE=.\include\pgCollection.h
# End Source File
# Begin Source File

SOURCE=.\include\pgColumn.h
# End Source File
# Begin Source File

SOURCE=.\include\pgConstraints.h
# End Source File
# Begin Source File

SOURCE=.\include\pgConversion.h
# End Source File
# Begin Source File

SOURCE=.\include\pgDatabase.h
# End Source File
# Begin Source File

SOURCE=.\include\pgDatatype.h
# End Source File
# Begin Source File

SOURCE=.\include\pgDomain.h
# End Source File
# Begin Source File

SOURCE=.\include\pgForeignKey.h
# End Source File
# Begin Source File

SOURCE=.\include\pgFunction.h
# End Source File
# Begin Source File

SOURCE=.\include\pgGroup.h
# End Source File
# Begin Source File

SOURCE=.\include\pgIndex.h
# End Source File
# Begin Source File

SOURCE=.\include\pgIndexConstraint.h
# End Source File
# Begin Source File

SOURCE=.\include\pgLanguage.h
# End Source File
# Begin Source File

SOURCE=.\include\pgObject.h
# End Source File
# Begin Source File

SOURCE=.\include\pgOperator.h
# End Source File
# Begin Source File

SOURCE=.\include\pgOperatorClass.h
# End Source File
# Begin Source File

SOURCE=.\include\pgRole.h
# End Source File
# Begin Source File

SOURCE=.\include\pgRule.h
# End Source File
# Begin Source File

SOURCE=.\include\pgSchema.h
# End Source File
# Begin Source File

SOURCE=.\include\pgSequence.h
# End Source File
# Begin Source File

SOURCE=.\include\pgServer.h
# End Source File
# Begin Source File

SOURCE=.\include\pgTable.h
# End Source File
# Begin Source File

SOURCE=.\include\pgTablespace.h
# End Source File
# Begin Source File

SOURCE=.\include\pgTrigger.h
# End Source File
# Begin Source File

SOURCE=.\include\pgType.h
# End Source File
# Begin Source File

SOURCE=.\include\pgUser.h
# End Source File
# Begin Source File

SOURCE=.\include\pgView.h
# End Source File
# End Group
# Begin Group "property includes"

# PROP Default_Filter "*.h"
# Begin Source File

SOURCE=.\include\dlgAggregate.h
# End Source File
# Begin Source File

SOURCE=.\include\dlgCast.h
# End Source File
# Begin Source File

SOURCE=.\include\dlgCheck.h
# End Source File
# Begin Source File

SOURCE=.\include\dlgColumn.h
# End Source File
# Begin Source File

SOURCE=.\include\dlgConversion.h
# End Source File
# Begin Source File

SOURCE=.\include\dlgDatabase.h
# End Source File
# Begin Source File

SOURCE=.\include\dlgDomain.h
# End Source File
# Begin Source File

SOURCE=.\include\dlgForeignKey.h
# End Source File
# Begin Source File

SOURCE=.\include\dlgFunction.h
# End Source File
# Begin Source File

SOURCE=.\include\dlgGroup.h
# End Source File
# Begin Source File

SOURCE=.\include\dlgIndex.h
# End Source File
# Begin Source File

SOURCE=.\include\dlgIndexConstraint.h
# End Source File
# Begin Source File

SOURCE=.\include\dlgLanguage.h
# End Source File
# Begin Source File

SOURCE=.\include\dlgOperator.h
# End Source File
# Begin Source File

SOURCE=.\include\dlgProperty.h
# End Source File
# Begin Source File

SOURCE=.\include\dlgRole.h
# End Source File
# Begin Source File

SOURCE=.\include\dlgRule.h
# End Source File
# Begin Source File

SOURCE=.\include\dlgSchema.h
# End Source File
# Begin Source File

SOURCE=.\include\dlgSequence.h
# End Source File
# Begin Source File

SOURCE=.\include\dlgServer.h
# End Source File
# Begin Source File

SOURCE=.\include\dlgTable.h
# End Source File
# Begin Source File

SOURCE=.\include\dlgTablespace.h
# End Source File
# Begin Source File

SOURCE=.\include\dlgTrigger.h
# End Source File
# Begin Source File

SOURCE=.\include\dlgType.h
# End Source File
# Begin Source File

SOURCE=.\include\dlgUser.h
# End Source File
# Begin Source File

SOURCE=.\include\dlgView.h
# End Source File
# End Group
# Begin Group "images"

# PROP Default_Filter "*.xpm"
# Begin Source File

SOURCE=.\include\images\aggregate.xpm
# End Source File
# Begin Source File

SOURCE=.\include\images\back.xpm
# End Source File
# Begin Source File

SOURCE=.\include\images\backup.xpm
# End Source File
# Begin Source File

SOURCE=.\include\images\baddatabase.xpm
# End Source File
# Begin Source File

SOURCE=.\include\images\cast.xpm
# End Source File
# Begin Source File

SOURCE=.\include\images\check.xpm
# End Source File
# Begin Source File

SOURCE=.\include\images\checked.xpm
# End Source File
# Begin Source File

SOURCE=.\include\images\clip_copy.xpm
# End Source File
# Begin Source File

SOURCE=.\include\images\clip_cut.xpm
# End Source File
# Begin Source File

SOURCE=.\include\images\clip_paste.xpm
# End Source File
# Begin Source File

SOURCE=.\include\images\closeddatabase.xpm
# End Source File
# Begin Source File

SOURCE=.\include\images\closedown.xpm
# End Source File
# Begin Source File

SOURCE=.\include\images\closeup.xpm
# End Source File
# Begin Source File

SOURCE=.\include\images\column.xpm
# End Source File
# Begin Source File

SOURCE=.\include\images\connect.xpm
# End Source File
# Begin Source File

SOURCE=.\include\images\constraints.xpm
# End Source File
# Begin Source File

SOURCE=.\include\images\conversion.xpm
# End Source File
# Begin Source File

SOURCE=.\include\images\create.xpm
# End Source File
# Begin Source File

SOURCE=.\include\images\database.xpm
# End Source File
# Begin Source File

SOURCE=.\include\images\delete.xpm
# End Source File
# Begin Source File

SOURCE=.\include\images\dnd_copy.xpm
# End Source File
# Begin Source File

SOURCE=.\include\images\dnd_move.xpm
# End Source File
# Begin Source File

SOURCE=.\include\images\dnd_none.xpm
# End Source File
# Begin Source File

SOURCE=.\include\images\domain.xpm
# End Source File
# Begin Source File

SOURCE=.\include\images\drop.xpm
# End Source File
# Begin Source File

SOURCE=.\include\images\edit_clear.xpm
# End Source File
# Begin Source File

SOURCE=.\include\images\edit_find.xpm
# End Source File
# Begin Source File

SOURCE=.\include\images\edit_redo.xpm
# End Source File
# Begin Source File

SOURCE=.\include\images\edit_undo.xpm
# End Source File
# Begin Source File

SOURCE=.\include\images\elephant32.xpm
# End Source File
# Begin Source File

SOURCE=.\include\images\elephant48.xpm
# End Source File
# Begin Source File

SOURCE=.\include\images\encoding.xpm
# End Source File
# Begin Source File

SOURCE=.\include\images\event.xpm
# End Source File
# Begin Source File

SOURCE=.\include\images\ex_aggregate.xpm
# End Source File
# Begin Source File

SOURCE=.\include\images\ex_append.xpm
# End Source File
# Begin Source File

SOURCE=.\include\images\ex_group.xpm
# End Source File
# Begin Source File

SOURCE=.\include\images\ex_hash.xpm
# End Source File
# Begin Source File

SOURCE=.\include\images\ex_join.xpm
# End Source File
# Begin Source File

SOURCE=.\include\images\ex_limit.xpm
# End Source File
# Begin Source File

SOURCE=.\include\images\ex_materialize.xpm
# End Source File
# Begin Source File

SOURCE=.\include\images\ex_merge.xpm
# End Source File
# Begin Source File

SOURCE=.\include\images\ex_nested.xpm
# End Source File
# Begin Source File

SOURCE=.\include\images\ex_result.xpm
# End Source File
# Begin Source File

SOURCE=.\include\images\ex_scan.xpm
# End Source File
# Begin Source File

SOURCE=.\include\images\ex_seek.xpm
# End Source File
# Begin Source File

SOURCE=.\include\images\ex_setop.xpm
# End Source File
# Begin Source File

SOURCE=.\include\images\ex_sort.xpm
# End Source File
# Begin Source File

SOURCE=.\include\images\ex_subplan.xpm
# End Source File
# Begin Source File

SOURCE=.\include\images\ex_unique.xpm
# End Source File
# Begin Source File

SOURCE=.\include\images\ex_unknown.xpm
# End Source File
# Begin Source File

SOURCE=.\include\images\file_open.xpm
# End Source File
# Begin Source File

SOURCE=.\include\images\file_save.xpm
# End Source File
# Begin Source File

SOURCE=.\include\images\foreignkey.xpm
# End Source File
# Begin Source File

SOURCE=.\include\images\forward.xpm
# End Source File
# Begin Source File

SOURCE=.\include\images\function.xpm
# End Source File
# Begin Source File

SOURCE=.\include\images\group.xpm
# End Source File
# Begin Source File

SOURCE=.\include\images\help.xpm
# End Source File
# Begin Source File

SOURCE=.\include\images\help2.xpm
# End Source File
# Begin Source File

SOURCE=.\include\images\hint.xpm
# End Source File
# Begin Source File

SOURCE=.\include\images\hint2.xpm
# End Source File
# Begin Source File

SOURCE=.\include\images\index.xpm
# End Source File
# Begin Source File

SOURCE=.\include\images\job.xpm
# End Source File
# Begin Source File

SOURCE=.\include\images\jobdisabled.xpm
# End Source File
# Begin Source File

SOURCE=.\include\images\key.xpm
# End Source File
# Begin Source File

SOURCE=.\include\images\language.xpm
# End Source File
# Begin Source File

SOURCE=.\include\images\namespace.xpm
# End Source File
# Begin Source File

SOURCE=.\include\images\operator.xpm
# End Source File
# Begin Source File

SOURCE=.\include\images\operatorclass.xpm
# End Source File
# Begin Source File

SOURCE=.\include\images\pgAdmin3.xpm
# End Source File
# Begin Source File

SOURCE=.\include\images\primarykey.xpm
# End Source File
# Begin Source File

SOURCE=.\include\images\properties.xpm
# End Source File
# Begin Source File

SOURCE=.\include\images\property.xpm
# End Source File
# Begin Source File

SOURCE=.\include\images\public.xpm
# End Source File
# Begin Source File

SOURCE=.\include\images\query_cancel.xpm
# End Source File
# Begin Source File

SOURCE=.\include\images\query_execfile.xpm
# End Source File
# Begin Source File

SOURCE=.\include\images\query_execute.xpm
# End Source File
# Begin Source File

SOURCE=.\include\images\query_explain.xpm
# End Source File
# Begin Source File

SOURCE=.\include\images\readdata.xpm
# End Source File
# Begin Source File

SOURCE=.\include\images\refresh.xpm
# End Source File
# Begin Source File

SOURCE=.\include\images\relationship.xpm
# End Source File
# Begin Source File

SOURCE=.\include\images\reload.xpm
# End Source File
# Begin Source File

SOURCE=.\include\images\restore.xpm
# End Source File
# Begin Source File

SOURCE=.\include\images\rule.xpm
# End Source File
# Begin Source File

SOURCE=.\include\images\schedule.xpm
# End Source File
# Begin Source File

SOURCE=.\include\images\sequence.xpm
# End Source File
# Begin Source File

SOURCE=.\include\images\server.xpm
# End Source File
# Begin Source File

SOURCE=.\include\images\serverbad.xpm
# End Source File
# Begin Source File

SOURCE=.\include\images\servers.xpm
# End Source File
# Begin Source File

SOURCE=.\include\images\slcluster.xpm
# End Source File
# Begin Source File

SOURCE=.\include\images\sllisten.xpm
# End Source File
# Begin Source File

SOURCE=.\include\images\slnode.xpm
# End Source File
# Begin Source File

SOURCE=.\include\images\slpath.xpm
# End Source File
# Begin Source File

SOURCE=.\include\images\slset.xpm
# End Source File
# Begin Source File

SOURCE=.\include\images\slsubscription.xpm
# End Source File
# Begin Source File

SOURCE=.\include\images\sortfilter.xpm
# End Source File
# Begin Source File

SOURCE=.\include\images\splash.xpm
# End Source File
# Begin Source File

SOURCE=.\include\images\sql.xpm
# End Source File
# Begin Source File

SOURCE=.\include\images\statistics.xpm
# End Source File
# Begin Source File

SOURCE=.\include\images\step.xpm
# End Source File
# Begin Source File

SOURCE=.\include\images\storage.xpm
# End Source File
# Begin Source File

SOURCE=.\include\images\storedata.xpm
# End Source File
# Begin Source File

SOURCE=.\include\images\table.xpm
# End Source File
# Begin Source File

SOURCE=.\include\images\tablespace.xpm
# End Source File
# Begin Source File

SOURCE=.\include\images\trigger.xpm
# End Source File
# Begin Source File

SOURCE=.\include\images\triggerfunction.xpm
# End Source File
# Begin Source File

SOURCE=.\include\images\type.xpm
# End Source File
# Begin Source File

SOURCE=.\include\images\unchecked.xpm
# End Source File
# Begin Source File

SOURCE=.\include\images\unique.xpm
# End Source File
# Begin Source File

SOURCE=.\include\images\unknown.xpm
# End Source File
# Begin Source File

SOURCE=.\include\images\user.xpm
# End Source File
# Begin Source File

SOURCE=.\include\images\vacuum.xpm
# End Source File
# Begin Source File

SOURCE=.\include\images\view.xpm
# End Source File
# Begin Source File

SOURCE=.\include\images\viewdata.xpm
# End Source File
# Begin Source File

SOURCE=.\include\images\viewfiltereddata.xpm
# End Source File
# End Group
# Begin Group "dummy includes"

# PROP Default_Filter "*.h"
# Begin Source File

SOURCE=.\include\parser\keywords.h
# End Source File
# Begin Source File

SOURCE=.\include\parser\parse.h
# End Source File
# Begin Source File

SOURCE=.\include\nodes\parsenodes.h
# End Source File
# Begin Source File

SOURCE=.\include\postgres.h
# End Source File
# Begin Source File

SOURCE=.\include\wxgridsel.h
# End Source File
# End Group
# Begin Group "agent includes"

# PROP Default_Filter "*.h"
# Begin Source File

SOURCE=.\agent\include\dlgJob.h
# End Source File
# Begin Source File

SOURCE=.\agent\include\dlgSchedule.h
# End Source File
# Begin Source File

SOURCE=.\agent\include\dlgStep.h
# End Source File
# Begin Source File

SOURCE=.\agent\include\pgaJob.h
# End Source File
# Begin Source File

SOURCE=.\agent\include\pgaSchedule.h
# End Source File
# Begin Source File

SOURCE=.\agent\include\pgaStep.h
# End Source File
# End Group
# Begin Group "misc includes"

# PROP Default_Filter "*.h"
# Begin Source File

SOURCE=.\include\copyright.h
# End Source File
# Begin Source File

SOURCE=.\include\languages.h
# End Source File
# Begin Source File

SOURCE=.\include\menu.h
# End Source File
# Begin Source File

SOURCE=.\include\misc.h
# End Source File
# Begin Source File

SOURCE=.\include\pgAdmin3.h
# End Source File
# Begin Source File

SOURCE=.\include\pgconfig.h
# End Source File
# Begin Source File

SOURCE=.\include\pgDefs.h
# End Source File
# Begin Source File

SOURCE=.\include\precomp.h
# End Source File
# Begin Source File

SOURCE=.\include\sysLogger.h
# End Source File
# Begin Source File

SOURCE=.\include\sysProcess.h
# End Source File
# Begin Source File

SOURCE=.\include\sysSettings.h
# End Source File
# Begin Source File

SOURCE=.\include\update.h
# End Source File
# Begin Source File

SOURCE=.\include\utffile.h
# End Source File
# Begin Source File

SOURCE=.\include\version.h
# End Source File
# End Group
# Begin Group "ui base includes"

# PROP Default_Filter "*.h"
# Begin Source File

SOURCE=.\include\ctl\calbox.h
# End Source File
# Begin Source File

SOURCE=.\include\ctl\ctlComboBox.h
# End Source File
# Begin Source File

SOURCE=.\include\ctl\ctlListView.h
# End Source File
# Begin Source File

SOURCE=.\include\ctlSecurityPanel.h
# End Source File
# Begin Source File

SOURCE=.\include\ctl\ctlSQLBox.h
# End Source File
# Begin Source File

SOURCE=.\include\ctl\ctlSQLResult.h
# End Source File
# Begin Source File

SOURCE=.\include\ctl\ctlTree.h
# End Source File
# Begin Source File

SOURCE=.\include\dlgClasses.h
# End Source File
# Begin Source File

SOURCE=.\include\explainCanvas.h
# End Source File
# Begin Source File

SOURCE=.\include\ctl\timespin.h
# End Source File
# Begin Source File

SOURCE=.\include\ctl\xh_calb.h
# End Source File
# Begin Source File

SOURCE=.\include\ctl\xh_ctlcombo.h
# End Source File
# Begin Source File

SOURCE=.\include\ctl\xh_sqlbox.h
# End Source File
# Begin Source File

SOURCE=.\include\ctl\xh_timespin.h
# End Source File
# End Group
# Begin Group "slony includes"

# PROP Default_Filter "*.h"
# Begin Source File

SOURCE=.\slony\include\dlgRepCluster.h
# End Source File
# Begin Source File

SOURCE=.\slony\include\dlgRepListen.h
# End Source File
# Begin Source File

SOURCE=.\slony\include\dlgRepNode.h
# End Source File
# Begin Source File

SOURCE=.\slony\include\dlgRepPath.h
# End Source File
# Begin Source File

SOURCE=.\slony\include\dlgRepProperty.h
# End Source File
# Begin Source File

SOURCE=.\slony\include\dlgRepSequence.h
# End Source File
# Begin Source File

SOURCE=.\slony\include\dlgRepSet.h
# End Source File
# Begin Source File

SOURCE=.\slony\include\dlgRepSubscription.h
# End Source File
# Begin Source File

SOURCE=.\slony\include\dlgRepTable.h
# End Source File
# Begin Source File

SOURCE=.\slony\include\slCluster.h
# End Source File
# Begin Source File

SOURCE=.\slony\include\slListen.h
# End Source File
# Begin Source File

SOURCE=.\slony\include\slNode.h
# End Source File
# Begin Source File

SOURCE=.\slony\include\slPath.h
# End Source File
# Begin Source File

SOURCE=.\slony\include\slSequence.h
# End Source File
# Begin Source File

SOURCE=.\slony\include\slSet.h
# End Source File
# Begin Source File

SOURCE=.\slony\include\slSubscription.h
# End Source File
# Begin Source File

SOURCE=.\slony\include\slTable.h
# End Source File
# End Group
# Begin Group "base includes"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\include\base\appbase.h
# End Source File
# Begin Source File

SOURCE=.\include\base\base.h
# End Source File
# Begin Source File

SOURCE=.\include\base\factory.h
# End Source File
# Begin Source File

SOURCE=.\include\base\pgConnBase.h
# End Source File
# Begin Source File

SOURCE=.\include\base\pgDefs.h
# End Source File
# Begin Source File

SOURCE=.\include\base\pgSetBase.h
# End Source File
# Begin Source File

SOURCE=.\include\base\sysLogger.h
# End Source File
# End Group
# Begin Group "db includes"

# PROP Default_Filter "*.h"
# Begin Source File

SOURCE=.\include\pgConn.h
# End Source File
# Begin Source File

SOURCE=.\include\pgSet.h
# End Source File
# End Group
# End Group
# Begin Group "agent"

# PROP Default_Filter "*.cpp"
# Begin Source File

SOURCE=.\agent\dlgJob.cpp
# End Source File
# Begin Source File

SOURCE=.\agent\dlgSchedule.cpp
# End Source File
# Begin Source File

SOURCE=.\agent\dlgStep.cpp
# End Source File
# Begin Source File

SOURCE=.\agent\pgaJob.cpp
# End Source File
# Begin Source File

SOURCE=.\agent\pgaSchedule.cpp
# End Source File
# Begin Source File

SOURCE=.\agent\pgaStep.cpp
# End Source File
# End Group
# Begin Group "slony"

# PROP Default_Filter "*.cpp"
# Begin Source File

SOURCE=.\slony\dlgRepCluster.cpp
# End Source File
# Begin Source File

SOURCE=.\slony\dlgRepListen.cpp
# End Source File
# Begin Source File

SOURCE=.\slony\dlgRepNode.cpp
# End Source File
# Begin Source File

SOURCE=.\slony\dlgRepPath.cpp
# End Source File
# Begin Source File

SOURCE=.\slony\dlgRepSequence.cpp
# End Source File
# Begin Source File

SOURCE=.\slony\dlgRepSet.cpp
# End Source File
# Begin Source File

SOURCE=.\slony\dlgRepSubscription.cpp
# End Source File
# Begin Source File

SOURCE=.\slony\dlgRepTable.cpp
# End Source File
# Begin Source File

SOURCE=.\slony\slCluster.cpp
# End Source File
# Begin Source File

SOURCE=.\slony\slListen.cpp
# End Source File
# Begin Source File

SOURCE=.\slony\slNode.cpp
# End Source File
# Begin Source File

SOURCE=.\slony\slPath.cpp
# End Source File
# Begin Source File

SOURCE=.\slony\slSequence.cpp
# End Source File
# Begin Source File

SOURCE=.\slony\slSet.cpp
# End Source File
# Begin Source File

SOURCE=.\slony\slSubscription.cpp
# End Source File
# Begin Source File

SOURCE=.\slony\slTable.cpp
# End Source File
# End Group
# Begin Group "unix build"

# PROP Default_Filter ""
# Begin Group "top level"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\acinclude.m4
# End Source File
# Begin Source File

SOURCE=..\bootstrap
# End Source File
# Begin Source File

SOURCE=..\configure.ac
# End Source File
# Begin Source File

SOURCE=..\Makefile.am
# End Source File
# End Group
# Begin Source File

SOURCE=.\Makefile.am
# End Source File
# End Group
# Begin Group "base"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\base\appbase.cpp
# SUBTRACT BASE CPP /YX /Yc /Yu
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\base\base.cpp
# SUBTRACT BASE CPP /YX /Yc /Yu
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\base\factory.cpp
# End Source File
# Begin Source File

SOURCE=.\base\pgConnBase.cpp
# SUBTRACT BASE CPP /YX /Yc /Yu
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\base\pgSetBase.cpp
# SUBTRACT BASE CPP /YX /Yc /Yu
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\base\sysLogger.cpp
# SUBTRACT BASE CPP /YX /Yc /Yu
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# End Group
# End Group
# Begin Group "misc files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\BUGS
# End Source File
# Begin Source File

SOURCE=..\CHANGELOG
# End Source File
# Begin Source File

SOURCE=..\i18n\pgadmin3.lng
# End Source File
# Begin Source File

SOURCE="..\programmers-readme.txt"
# End Source File
# Begin Source File

SOURCE=..\TODO
# End Source File
# End Group
# End Target
# End Project
