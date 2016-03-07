#!/bin/sh
DMG_SOURCES="./pgAdmin3.app"
DMG_LICENCE=./pkg/mac/licence.r
DMG_IMAGE=./pgAdmin3.dmg
DMG_NAME=pgAdmin3
HDIUTIL=/usr/bin/hdiutil
REZ="/usr/bin/Rez /System/Library/Frameworks/Carbon.framework/Versions/A/Headers/*.r"

DMG_DIR=$DMG_IMAGE.src

if test -e "$DMG_DIR"; then
	echo "Directory $DMG_DIR already exists. Please delete it manually." >&2
	exit 1
fi

echo "Cleaning up"
rm -f "$DMG_IMAGE" || exit 1
mkdir "$DMG_DIR" || exit 1

echo "Copying data into temporary directory"
for src in $DMG_SOURCES; do
	cp -R "$src" "$DMG_DIR" || exit 1
done

echo "Creating image"
$HDIUTIL create -quiet -srcfolder "$DMG_DIR" -format UDZO -volname "$DMG_NAME" -ov "$DMG_IMAGE" || exit 1
rm -rf "$DMG_DIR" || exit 1

echo "Attaching License to image"
$HDIUTIL unflatten -quiet "$DMG_IMAGE" || exit 1
$REZ "$DMG_LICENCE" -a -o "$DMG_IMAGE" || exit 1
$HDIUTIL flatten -quiet "$DMG_IMAGE" || exit 1
