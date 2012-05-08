Set objFS = CreateObject("Scripting.FileSystemObject")

' *********** Read the version from version.h ***********
strFile= "..\pgadmin\include\version.h"
Set objFile = objFS.OpenTextFile(strFile, 1)
Do Until objFile.AtEndOfStream
    strLine = objFile.ReadLine
    If InStr(strLine,"#define VERSION_PACKAGE")> 0 then
	strLongToken= split(strLine)
	strLongVer = strLongToken(2)
    End If
Loop
objFile.Close
strShortToken = split(strLongVer, ".")
strShortVer = strShortToken(0) & "." & strShortToken(1)

' ********** Copy the conf.py.in file as conf.py *********
objFS.Copyfile "en_US\conf.py.in" , "en_US\conf.py"

' ****** Replace the Long and Short version in conf.py file ******
strConfFile = "en_US\conf.py"
Set objConfFile = objFS.OpenTextFile(strConfFile, 1)
strText = objConfFile.ReadAll
objConfFile.Close
strText = Replace(strText, "PGADMIN_LONG_VERSION", strLongVer)
strText = Replace(strText, "PGADMIN_SHORT_VERSION", strShortVer)
Set objConfFile = objFS.OpenTextFile(strConfFile, 2)
objConfFile.WriteLine strText
objConfFile.Close