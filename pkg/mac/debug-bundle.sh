#!/bin/bash
bundle_name="pgAdmin3-Debug"

if test -d "$bundle_name.app"; then
	echo "Bundle: $bundle_name.app already exists" >&2
	exit 1
fi

mkdir "$bundle_name.app"
mkdir -p "$bundle_name.app/Contents/MacOS"
mkdir -p "$bundle_name.app/Contents/Resources"
mkdir -p "$bundle_name.app/Contents/SharedSupport"

(cd "$bundle_name.app/Contents"; ln -s ../../pkg/mac/PkgInfo PkgInfo) &&
(cd "$bundle_name.app/Contents" && ln -s ../../pkg/mac/debug.pgadmin.Info.plist Info.plist) &&
(cd "$bundle_name.app/Contents/MacOS" && ln -s ../../../pgadmin/pgadmin3 "$bundle_name") &&
(cd "$bundle_name.app/Contents/Resources" && ln -s "../../../pkg/mac/pgadmin3.icns" "$bundle_name.icns") &&
(cd "$bundle_name.app/Contents/SharedSupport" && ln -s ../../../pgadmin/ui ui) &&
(cd "$bundle_name.app/Contents/SharedSupport" && ln -s ../../../docs docs) &&
(cd "$bundle_name.app/Contents/SharedSupport" && ln -s ../../../i18n i18n)
