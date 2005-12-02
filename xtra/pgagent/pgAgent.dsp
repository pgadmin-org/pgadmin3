# Microsoft Developer Studio Project File - Name="pgAgent" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=pgAgent - Win32 Release
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "pgAgent.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "pgAgent.mak" CFG="pgAgent - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "pgAgent - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE "pgAgent - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "pgAgent - Win32 Debug"

# PROP BASE Use_MFC 1
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
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "include" /I "c:/program files/postgresql/8.1/include" /I "c:/wxWidgets-2.6/include" /I "c:/wxWidgets-2.6/contrib/include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "__WINDOWS__" /D "__WXMSW__" /D DEBUG=1 /D "__WXDEBUG__" /D "__WIN95__" /D "__WIN32__" /D WINVER=0x0400 /D "STRICT" /D wxUSE_UNICODE=1 /D "_UNICODE" /D "UNICODE" /D "SSL" /FAcs /Fr /Yu"pgAgent.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x407 /d "_DEBUG"
# ADD RSC /l 0x809 /i "c:/wxWidgets-2.6/include" /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 libpq.lib wxbase26ud.lib wxbase26ud_xml.lib wxbase26ud_net.lib wxmsw26ud_adv.lib wxmsw26ud_core.lib wxmsw26ud_html.lib wxregexud.lib wxpngd.lib wxzlibd.lib wxjpegd.lib wxtiffd.lib wxmsw26ud_stc.lib wxmsw26ud_ogl.lib wxmsw26ud_xrc.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib uuid.lib comctl32.lib rpcrt4.lib wsock32.lib winmm.lib ole32.lib oleaut32.lib /nologo /subsystem:console /map /debug /machine:I386 /nodefaultlib:"libcd.lib" /nodefaultlib:"libcid.lib" /nodefaultlib:"msvcrt.lib" /pdbtype:sept /libpath:"c:/wxWidgets-2.6/lib/vc_lib" /libpath:"c:/program files/postgresql/8.1/lib/ms"
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "pgAgent - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "pgAgent___Win32_Release"
# PROP BASE Intermediate_Dir "pgAgent___Win32_Release"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /I "include" /I "c:/program files/postgresql/8.0/include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_MT" /Yu"pgAgent.h" /FD /GZ /c
# ADD CPP /nologo /MD /W3 /GX /O2 /Ob2 /I "include" /I "c:/program files/postgresql/8.1/include" /I "c:/wxWidgets-2.6/include" /I "c:/wxWidgets-2.6/contrib/include" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "__WINDOWS__" /D "__WIN95__" /D "__WIN32__" /D WINVER=0x0400 /D "STRICT" /D wxUSE_UNICODE=1 /D "_UNICODE" /D "UNICODE" /D "EMBED_XRC" /D "SSL" /FAcs /FR /Yu"pgAgent.h" /FD /c
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x407 /d "_DEBUG"
# ADD RSC /l 0x809 /i "c:/wxWidgets-2.6/include" /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib advapi32.lib user32.lib gdi32.lib wsock32.lib libpq.lib msvcrtd.lib oldnames.lib libcpmtd.lib /nologo /subsystem:console /debug /machine:I386 /nodefaultlib /pdbtype:sept /libpath:"c:/program files/postgresql/8.0/lib/ms"
# SUBTRACT BASE LINK32 /pdb:none
# ADD LINK32 libpq.lib wxbase26u.lib wxbase26u_xml.lib wxbase26u_net.lib wxmsw26u_adv.lib wxmsw26u_core.lib wxmsw26u_html.lib wxregexu.lib wxpng.lib wxzlib.lib wxjpeg.lib wxtiff.lib wxmsw26u_stc.lib wxmsw26u_ogl.lib wxmsw26u_xrc.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib uuid.lib comctl32.lib rpcrt4.lib wsock32.lib winmm.lib ole32.lib oleaut32.lib /nologo /subsystem:console /map /machine:I386 /nodefaultlib:"libcd.lib" /nodefaultlib:"libcid.lib" /nodefaultlib:"msvcrtd.lib" /pdbtype:sept /libpath:"c:/wxWidgets-2.6/lib/vc_lib" /libpath:"c:/program files/postgresql/8.1/lib/ms"
# SUBTRACT LINK32 /pdb:none

!ENDIF 

# Begin Target

# Name "pgAgent - Win32 Debug"
# Name "pgAgent - Win32 Release"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\connection.cpp
# End Source File
# Begin Source File

SOURCE=.\job.cpp
# End Source File
# Begin Source File

SOURCE=.\misc.cpp
# End Source File
# Begin Source File

SOURCE=.\pgAgent.cpp
# End Source File
# Begin Source File

SOURCE=.\precomp.cpp
# ADD CPP /Yc"pgAgent.h"
# End Source File
# Begin Source File

SOURCE=.\unix.cpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\win32.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\include\connection.h
# End Source File
# Begin Source File

SOURCE=.\include\job.h
# End Source File
# Begin Source File

SOURCE=.\include\misc.h
# End Source File
# Begin Source File

SOURCE=.\include\pgAgent.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\pgAgent.rc
# End Source File
# End Group
# Begin Group "Misc Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\pgagent.sql
# End Source File
# End Group
# End Target
# End Project
