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
# ADD CPP /nologo /MD /W3 /GX /O1 /Ob2 /I "c:/wx2/include" /I "c:/wx2/contrib/include" /I "c:/wx2/lib/mswd" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "__WINDOWS__" /D "__WXMSW__" /D "__WIN95__" /D "__WIN32__" /D WINVER=0x0400 /D "STRICT" /FD /c
# SUBTRACT CPP /YX
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x809 /d "NDEBUG"
# ADD RSC /l 0x809 /i "c:/wx2/include" /i "c:/wx2/contrib/include" /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib wsock32.lib winmm.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib comctl32.lib rpcrt4.lib wsock32.lib winmm.lib wxmsw.lib png.lib zlib.lib jpeg.lib tiff.lib stc.lib /nologo /subsystem:windows /machine:I386 /nodefaultlib:"libc.lib" /nodefaultlib:"libci.lib" /nodefaultlib:"msvcrtd.lib" /libpath:"c:/wx2/lib" /libpath:"c:/wx2/contrib/lib"

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
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "c:/wx2/include" /I "c:/wx2/contrib/include" /I "c:/wx2/lib/mswd" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "__WINDOWS__" /D "__WXMSW__" /D DEBUG=1 /D "__WXDEBUG__" /D "__WIN95__" /D "__WIN32__" /D WINVER=0x0400 /D "STRICT" /FR /FD /c
# SUBTRACT CPP /YX /Yc /Yu
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x809 /d "_DEBUG"
# ADD RSC /l 0x809 /i "c:/wx2/include" /i "c:/wx2/contrib/include" /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib wsock32.lib winmm.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib comctl32.lib rpcrt4.lib wsock32.lib winmm.lib wxmswd.lib pngd.lib zlibd.lib jpegd.lib tiffd.lib stcd.lib /nologo /subsystem:windows /debug /machine:I386 /nodefaultlib:"libcd.lib" /nodefaultlib:"libcid.lib" /nodefaultlib:"msvcrt.lib" /pdbtype:sept /libpath:"c:/wx2/lib" /libpath:"c:/wx2/contrib/lib"

!ENDIF 

# Begin Target

# Name "pgAdmin3 - Win32 Release"
# Name "pgAdmin3 - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "c,cpp"
# Begin Source File

SOURCE=.\ui\controls\ctlSQLBox.cpp
# End Source File
# Begin Source File

SOURCE=.\ui\forms\frmAbout.cpp
# End Source File
# Begin Source File

SOURCE=.\ui\forms\frmMain.cpp
# End Source File
# Begin Source File

SOURCE=.\ui\forms\frmSplash.cpp
# End Source File
# Begin Source File

SOURCE=.\pgAdmin3.cpp

!IF  "$(CFG)" == "pgAdmin3 - Win32 Release"

!ELSEIF  "$(CFG)" == "pgAdmin3 - Win32 Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\utils\sysLogger.cpp
# End Source File
# Begin Source File

SOURCE=.\utils\sysSettings.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h"
# Begin Source File

SOURCE=.\ui\controls\ctlSQLBox.h
# End Source File
# Begin Source File

SOURCE=.\ui\forms\frmAbout.h
# End Source File
# Begin Source File

SOURCE=.\ui\forms\frmMain.h
# End Source File
# Begin Source File

SOURCE=.\ui\forms\frmSplash.h
# End Source File
# Begin Source File

SOURCE=.\pgAdmin3.h
# End Source File
# Begin Source File

SOURCE=.\utils\sysLogger.h
# End Source File
# Begin Source File

SOURCE=.\utils\sysSettings.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "rc"
# Begin Source File

SOURCE=.\ui\pgAdmin3.rc
# End Source File
# End Group
# Begin Group "Unix"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Makefile
# End Source File
# End Group
# Begin Group "Images"

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

SOURCE=.\images\encoding.xpm
# End Source File
# Begin Source File

SOURCE=.\images\event.xpm
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

SOURCE=.\images\record.xpm
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

SOURCE=.\images\splash.xpm
# End Source File
# Begin Source File

SOURCE=.\images\sql.xpm
# End Source File
# Begin Source File

SOURCE=.\images\statistics.xpm
# End Source File
# Begin Source File

SOURCE=.\images\stop.xpm
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

SOURCE=.\images\upgrade.xpm
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
# End Target
# End Project
