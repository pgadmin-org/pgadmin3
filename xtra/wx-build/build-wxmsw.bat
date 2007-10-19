@echo off

setlocal

REM Configure which modules should be built
set WXBASE=adv aui base core html net wxexpat wxjpeg wxpng wxregex wxtiff wxzlib xml xrc
set WXCONTRIB=ogl stc

REM Location of wxWidgets source
set WX=%WXWIN%
set HERE=%CD%

if not exist %WX%\build\msw\wx.dsw goto no_wx

REM Copy include files
copy setup0-msw-2.8.h %WX%\include\wx\setup0.h
copy setup0-msw-2.8.h %WX%\include\wx\setup.h
copy setup0-msw-2.8.h %WX%\include\wx\msw\setup.h

REM Convert projects if necessary
cd %WX%\build\msw
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
cd %HERE%

REM Now build them
cd %WX%\build\msw
for %%b in (Debug Release) do (
   for %%f in (%WXBASE%) do (
      title Building project %%f, config %%b
      vcbuild /nohtmllog /nologo wx_%%f.vcproj "Unicode %%b"
   )
)
cd ..\..\contrib\build
for %%b in (Debug Release) do (
   for %%f in (%WXCONTRIB%) do (
      cd %%f
      title Building project contrib/%%f, config %%b
      vcbuild /nohtmllog /nologo %%f.vcproj "Unicode %%b"
      cd ..
   )
)
cd %HERE%
title "build-wx done."
echo "build-wx done."

goto :eof

:no_wx
echo wxWidgets not found in %WX%!
exit 1
