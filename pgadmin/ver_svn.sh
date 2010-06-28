#!/bin/sh

OV=unknown
if [ -d .svn -o -d ../.git ]; then
   if [ -f include/svnversion.h ]; then
      OV=$(cat include/svnversion.h | sed -e 's/#define VERSION_SVN "\(.*\)"/\1/')
   fi
fi

if [ -d .svn ]; then
   NV=$(svnversion ..)
elif [ -d ../.git ]; then
   NV=$(git describe --always)
fi


if [ "$OV" != "$NV" ]; then
   echo Refreshing svnversion.h
   echo "#define VERSION_SVN \"$NV\"" > include/svnversion.h
fi
