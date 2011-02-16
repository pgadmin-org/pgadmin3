@echo off

setlocal

REM Configure which modules should be built
set WXBASE=wxtiff wxexpat wxjpeg wxpng wxregex wxzlib base core adv aui html net xml xrc
set WXCONTRIB=stc

REM Location of wxWidgets source
set WX=%WXWIN%
set HERE=%CD%

if not exist %WX%\build\msw\wx.dsw goto no_wx

REM Copy include files
copy /Y setup0-msw-2.8.h "%WX%\include\wx\setup0.h"
copy /Y setup0-msw-2.8.h "%WX%\include\wx\setup.h"
copy /Y setup0-msw-2.8.h "%WX%\include\wx\msw\setup.h"

REM Convert projects if necessary
cd /D %WX%\build\msw
del *.vcproj.user 2> NUL
for %%f in (%WXBASE%) do (
   echo Checking %%f
   if not exist wx_%%f.vcproj vcbuild /nologo /upgrade wx_%%f.dsp
)
cd ..\..\contrib\build
for %%f in (%WXCONTRIB%) do (
   echo Checking contrib/%%f
   cd %%f
   del *.vcproj.user 2> NUL
   if not exist %%f.vcproj vcbuild /nologo /upgrade %%f.dsp
   cd ..
)

cd ..\..\utils\hhp2cached
echo Checking utils\hhp2cached
del *.vcproj.user 2> NUL
if not exist hhp2cached.vcproj vcbuild /nologo /upgrade hhp2cached.dsp

cd ..\wxrc
echo Checking utils\wxrc
del *.vcproj.user 2> NUL
if not exist wxrc.vcproj vcbuild /nologo /upgrade wxrc.dsp

REM Now build them
cd /D %WX%\build\msw
for %%b in (Debug Release) do (
   for %%f in (%WXBASE%) do (
      title Building project %%f, config %%b
      vcbuild /nohtmllog /nologo wx_%%f.vcproj "Unicode %%b"
      vcbuild /nohtmllog /nologo wx_%%f.vcproj "DLL Unicode %%b"
   )
)
cd ..\..\contrib\build
for %%b in (Debug Release) do (
   for %%f in (%WXCONTRIB%) do (
      cd %%f
      title Building project contrib/%%f, config %%b
      vcbuild /nohtmllog /nologo %%f.vcproj "Unicode %%b"
      vcbuild /nohtmllog /nologo %%f.vcproj "DLL Unicode %%b"
      cd ..
   )
)

cd ..\..\utils\hhp2cached
title Building project utils/hhp2cached, config Release
vcbuild /nohtmllog /nologo hhp2cached.vcproj "Unicode Release"

cd ..\wxrc
title Building project utils/wxrc, config Release
vcbuild /nohtmllog /nologo wxrc.vcproj "Unicode Release"

cd /D %HERE%
title "build-wx done."
echo "build-wx done."

goto :eof

:no_wx
echo wxWidgets not found in %WX%!
exit /b 1
