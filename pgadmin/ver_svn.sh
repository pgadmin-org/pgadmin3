#!/bin/bash

OV=unknown
if [ -d .svn ]; then
   if [ -f include/svnversion.h ]; then
      OV=$(cat include/svnversion.h | sed -e 's/#define VERSION_SVN "\(.*\)"/\1/')
   fi
fi

NV=$(svnversion ..)

if [ "$OV" != "$NV" ]; then
   echo Refreshing svnversion.h
   echo "#define VERSION_SVN \"$NV\"" > include/svnversion.h
fi
