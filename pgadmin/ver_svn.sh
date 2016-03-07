#!/bin/sh

TARGET_DIR=`dirname $0`

cd $TARGET_DIR
OV=unknown
if [ -d .svn -o -d ../.git ]; then
   if [ -f include/svnversion.h ]; then
      OV=$(cat include/svnversion.h | sed -e 's/#define VERSION_SVN "\(.*\)"/\1/')
   fi
else
   if [ ! -f include/svnversion.h ]; then
       echo Creating svnversion.h with version: unknown
       echo "#define VERSION_SVN \"unknown\"" > include/svnversion.h
   fi

   exit
fi

if [ -d .svn ]; then
   NV=$(svnversion ..)
elif [ -d ../.git ]; then
   NV=$(git describe --always)
fi


if [ "$OV" != "$NV" ]; then
   echo Refreshing svnversion.h with version: $NV
   echo "#define VERSION_SVN \"$NV\"" > include/svnversion.h
fi
