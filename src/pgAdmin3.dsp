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
!MESSAGE "pgAdmin3 - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "pgAdmin3 - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "pgAdmin3 - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /Ob2 /I "include/" /I "c:/wxWindows_2.4.0/include" /I "c:/wxWindows_2.4.0/contrib/include" /I "c:/wxWindows_2.4.0/lib/mswd" /I "c:/postgresql/include" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "__WINDOWS__" /D "__WXMSW__" /D "__WIN95__" /D "__WIN32__" /D WINVER=0x0400 /D "STRICT" /FAcs /FR /FD /c
# SUBTRACT CPP /YX /Yc /Yu
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x809 /d "NDEBUG"
# ADD RSC /l 0x809 /i "c:/wxWindows_2.4.0/include" /i "c:/wxWindows_2.4.0/contrib/include" /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib wsock32.lib winmm.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib comctl32.lib rpcrt4.lib wsock32.lib winmm.lib wxmsw.lib png.lib zlib.lib jpeg.lib tiff.lib stc.lib libpq.lib wxxrc.lib /nologo /subsystem:windows /map /machine:I386 /nodefaultlib:"libc.lib" /nodefaultlib:"libci.lib" /nodefaultlib:"msvcrtd.lib" /libpath:"c:/wxWindows_2.4.0/lib" /libpath:"c:/wxWindows_2.4.0/contrib/lib" /libpath:"c:/postgresql/lib"
# SUBTRACT LINK32 /debug

!ELSEIF  "$(CFG)" == "pgAdmin3 - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "include/" /I "c:/wxWindows_2.4.0/include" /I "c:/wxWindows_2.4.0/contrib/include" /I "c:/wxWindows_2.4.0/lib/mswd" /I "c:/postgresql/include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "__WINDOWS__" /D "__WXMSW__" /D DEBUG=1 /D "__WXDEBUG__" /D "__WIN95__" /D "__WIN32__" /D WINVER=0x0400 /D "STRICT" /FAcs /FR /FD /c
# SUBTRACT CPP /YX /Yc /Yu
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x809 /d "_DEBUG"
# ADD RSC /l 0x809 /i "c:/wxWindows_2.4.0/include" /i "c:/wxWindows_2.4.0/contrib/include" /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib wsock32.lib winmm.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib comctl32.lib rpcrt4.lib wsock32.lib winmm.lib wxmswd.lib pngd.lib zlibd.lib jpegd.lib tiffd.lib stcd.lib libpq.lib wxxrcd.lib /nologo /subsystem:windows /map /debug /machine:I386 /nodefaultlib:"libcd.lib" /nodefaultlib:"libcid.lib" /nodefaultlib:"msvcrt.lib" /pdbtype:sept /libpath:"c:/wxWindows_2.4.0/lib" /libpath:"c:/wxWindows_2.4.0/contrib/lib" /libpath:"c:/postgresql/lib"

!ENDIF 

# Begin Target

# Name "pgAdmin3 - Win32 Release"
# Name "pgAdmin3 - Win32 Debug"
# Begin Group "src"

# PROP Default_Filter ""
# Begin Group "db"

# PROP Default_Filter ""
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

SOURCE=.\schema\pgConversion.cpp
# End Source File
# Begin Source File

SOURCE=.\schema\pgDatabase.cpp
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

SOURCE=.\schema\pgLanguage.cpp
# End Source File
# Begin Source File

SOURCE=.\schema\pgObject.cpp

!IF  "$(CFG)" == "pgAdmin3 - Win32 Release"

# ADD CPP /O2

!ELSEIF  "$(CFG)" == "pgAdmin3 - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\schema\pgOperator.cpp
# End Source File
# Begin Source File

SOURCE=.\schema\pgOperatorClass.cpp
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

SOURCE=.\utils\sysLogger.cpp
# End Source File
# Begin Source File

SOURCE=.\utils\sysSettings.cpp
# End Source File
# End Group
# Begin Group "ui"

# PROP Default_Filter ""
# Begin Group "win32"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\ui\win32\frmConnect.xrc
# End Source File
# Begin Source File

SOURCE=.\ui\win32\frmOptions.xrc
# End Source File
# Begin Source File

SOURCE=.\ui\win32\frmPassword.xrc
# End Source File
# End Group
# Begin Group "gtk"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\ui\gtk\frmConnect.xrc
# End Source File
# Begin Source File

SOURCE=.\ui\gtk\frmOptions.xrc
# End Source File
# Begin Source File

SOURCE=.\ui\gtk\frmPassword.xrc
# End Source File
# End Group
# Begin Group "Query Builder"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\ui\dlgAddTableView.cpp
# End Source File
# Begin Source File

SOURCE=.\ui\frmChildTableViewFrame.cpp
# End Source File
# Begin Source File

SOURCE=.\ui\frmQBJoin.cpp
# End Source File
# Begin Source File

SOURCE=.\ui\frmQueryBuilder.cpp
# End Source File
# End Group
# Begin Group "Properties"

# PROP Default_Filter "*.cpp"
# Begin Source File

SOURCE=.\ui\dlgDatabase.cpp
# End Source File
# Begin Source File

SOURCE=.\ui\dlgProperty.cpp
# End Source File
# Begin Source File

SOURCE=.\ui\dlgUser.cpp
# End Source File
# End Group
# Begin Source File

SOURCE=.\ui\ctlSQLBox.cpp
# End Source File
# Begin Source File

SOURCE=.\ui\ctlSQLResult.cpp
# End Source File
# Begin Source File

SOURCE=.\ui\events.cpp
# End Source File
# Begin Source File

SOURCE=.\ui\frmAbout.cpp
# End Source File
# Begin Source File

SOURCE=.\ui\frmConnect.cpp
# End Source File
# Begin Source File

SOURCE=.\ui\frmEditGrid.cpp
# End Source File
# Begin Source File

SOURCE=.\ui\frmMain.cpp
# End Source File
# Begin Source File

SOURCE=.\ui\frmOptions.cpp
# End Source File
# Begin Source File

SOURCE=.\ui\frmPassword.cpp
# End Source File
# Begin Source File

SOURCE=.\ui\frmQuery.cpp
# End Source File
# Begin Source File

SOURCE=.\ui\frmSplash.cpp
# End Source File
# Begin Source File

SOURCE=.\ui\frmStatus.cpp
# End Source File
# Begin Source File

SOURCE=.\ui\frmVacuum.cpp
# End Source File
# End Group
# Begin Group "include"

# PROP Default_Filter ""
# Begin Group "images"

# PROP Default_Filter "xpm"
# Begin Source File

SOURCE=.\images\aggregate.xpm
# End Source File
# Begin Source File

SOURCE=.\images\arguments.xpm
# End Source File
# Begin Source File

SOURCE=.\images\baddatabase.xpm
# End Source File
# Begin Source File

SOURCE=.\images\check.xpm
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

SOURCE=.\images\closeddatabase.xpm
# End Source File
# Begin Source File

SOURCE=.\images\column.xpm
# End Source File
# Begin Source File

SOURCE=.\images\connect.xpm
# End Source File
# Begin Source File

SOURCE=.\images\create.xpm
# End Source File
# Begin Source File

SOURCE=.\images\database.xpm
# End Source File
# Begin Source File

SOURCE=.\images\domain.xpm
# End Source File
# Begin Source File

SOURCE=.\images\drop.xpm
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

SOURCE=.\images\encoding.xpm
# End Source File
# Begin Source File

SOURCE=.\images\event.xpm
# End Source File
# Begin Source File

SOURCE=.\include\images\file_open.xpm
# End Source File
# Begin Source File

SOURCE=.\include\images\file_save.xpm
# End Source File
# Begin Source File

SOURCE=.\images\function.xpm
# End Source File
# Begin Source File

SOURCE=.\images\group.xpm
# End Source File
# Begin Source File

SOURCE=.\images\hiproperty.xpm
# End Source File
# Begin Source File

SOURCE=.\images\index.xpm
# End Source File
# Begin Source File

SOURCE=.\images\indexcolumn.xpm
# End Source File
# Begin Source File

SOURCE=.\images\language.xpm
# End Source File
# Begin Source File

SOURCE=.\images\namespace.xpm
# End Source File
# Begin Source File

SOURCE=.\images\operator.xpm
# End Source File
# Begin Source File

SOURCE=.\images\pgAdmin3.ico
# End Source File
# Begin Source File

SOURCE=.\images\pgAdmin3.xpm
# End Source File
# Begin Source File

SOURCE=.\images\properties.xpm
# End Source File
# Begin Source File

SOURCE=.\images\property.xpm
# End Source File
# Begin Source File

SOURCE=.\images\public.xpm
# End Source File
# Begin Source File

SOURCE=.\include\images\query_cancel.xpm
# End Source File
# Begin Source File

SOURCE=.\include\images\query_execute.xpm
# End Source File
# Begin Source File

SOURCE=.\include\images\query_explain.xpm
# End Source File
# Begin Source File

SOURCE=.\images\refresh.xpm
# End Source File
# Begin Source File

SOURCE=.\images\relationship.xpm
# End Source File
# Begin Source File

SOURCE=.\images\rule.xpm
# End Source File
# Begin Source File

SOURCE=.\images\sequence.xpm
# End Source File
# Begin Source File

SOURCE=.\images\server.xpm
# End Source File
# Begin Source File

SOURCE=.\images\serverbad.xpm
# End Source File
# Begin Source File

SOURCE=.\images\splash.xpm
# End Source File
# Begin Source File

SOURCE=.\images\sql.xpm
# End Source File
# Begin Source File

SOURCE=.\images\statistics.xpm
# End Source File
# Begin Source File

SOURCE=.\images\storage.xpm
# End Source File
# Begin Source File

SOURCE=.\images\table.xpm
# End Source File
# Begin Source File

SOURCE=.\images\trigger.xpm
# End Source File
# Begin Source File

SOURCE=.\images\type.xpm
# End Source File
# Begin Source File

SOURCE=.\images\unknown.xpm
# End Source File
# Begin Source File

SOURCE=.\images\user.xpm
# End Source File
# Begin Source File

SOURCE=.\images\vacuum.xpm
# End Source File
# Begin Source File

SOURCE=.\images\view.xpm
# End Source File
# Begin Source File

SOURCE=.\images\viewdata.xpm
# End Source File
# End Group
# Begin Group "ui includes"

# PROP Default_Filter "*.h"
# Begin Source File

SOURCE=.\include\ctlSQLBox.h
# End Source File
# Begin Source File

SOURCE=.\include\ctlSQLResult.h
# End Source File
# Begin Source File

SOURCE=.\include\dlgAddTableView.h
# End Source File
# Begin Source File

SOURCE=.\include\frmAbout.h
# End Source File
# Begin Source File

SOURCE=.\include\frmChildTableViewFrame.h
# End Source File
# Begin Source File

SOURCE=.\include\frmConnect.h
# End Source File
# Begin Source File

SOURCE=.\include\frmEditGrid.h
# End Source File
# Begin Source File

SOURCE=.\include\frmMain.h
# End Source File
# Begin Source File

SOURCE=.\include\frmOptions.h
# End Source File
# Begin Source File

SOURCE=.\include\frmPassword.h
# End Source File
# Begin Source File

SOURCE=.\include\frmQBJoin.h
# End Source File
# Begin Source File

SOURCE=.\include\frmQuery.h
# End Source File
# Begin Source File

SOURCE=.\include\frmQueryBuilder.h
# End Source File
# Begin Source File

SOURCE=.\include\frmSplash.h
# End Source File
# Begin Source File

SOURCE=.\include\frmStatus.h
# End Source File
# Begin Source File

SOURCE=.\include\frmVacuum.h
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

SOURCE=.\include\pgConn.h
# End Source File
# Begin Source File

SOURCE=.\include\pgConversion.h
# End Source File
# Begin Source File

SOURCE=.\include\pgDatabase.h
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

SOURCE=.\include\pgSet.h
# End Source File
# Begin Source File

SOURCE=.\include\pgTable.h
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

SOURCE=.\include\dlgDatabase.h
# End Source File
# Begin Source File

SOURCE=.\include\dlgProperty.h
# End Source File
# Begin Source File

SOURCE=.\include\dlgUser.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\include\misc.h
# End Source File
# Begin Source File

SOURCE=.\include\pgAdmin3.h
# End Source File
# Begin Source File

SOURCE=.\include\sysLogger.h
# End Source File
# Begin Source File

SOURCE=.\include\sysSettings.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\Makefile
# End Source File
# Begin Source File

SOURCE=.\pgAdmin3.cpp

!IF  "$(CFG)" == "pgAdmin3 - Win32 Release"

!ELSEIF  "$(CFG)" == "pgAdmin3 - Win32 Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# End Group
# Begin Group "misc files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\BUGS.txt
# End Source File
# Begin Source File

SOURCE=.\ui\pgAdmin3.rc
# End Source File
# Begin Source File

SOURCE=.\tips.txt
# End Source File
# Begin Source File

SOURCE=..\TODO.txt
# End Source File
# End Group
# End Target
# End Project
