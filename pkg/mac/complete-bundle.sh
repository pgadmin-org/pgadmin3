#!/bin/sh
bundle="$1"

if ! test -d "$bundle" ; then
	echo "$bundle is no bundle!" >&2
	exit 1
fi

test -d "$bundle/Contents/Frameworks" || mkdir -p "$bundle/Contents/Frameworks" || exit 1

echo "Completing bundle: $bundle"
cd "$bundle"
fw_basepath=$(dirname $(pwd))
todo=$(find ./ | \
        xargs --replace=line file 'line' | \
        sed -n 's/^\([^:][^:]*\):[[:space:]]*Mach-O executable ppc$/\1/p' | \
        xargs echo -n \
)

echo "Found executables: $todo"
while test "$todo" != ""; do
	todo_old=$todo ;
	todo="" ;
	for todo_obj in $todo_old; do
		for lib in $(
			otool -L $todo_obj | \
			sed -n 's|^.*[[:space:]]\([^[:space:]]*\.dylib\).*$|\1|p' | \
			egrep -v '^(/usr/lib)|(/System)|@executable_path@' \
		); do
			lib_bn="$(basename "$lib")" ;
			if ! test -f "Contents/Frameworks/$lib_bn"; then
				echo "Adding library: $lib_bn (because of: $todo_obj)"
				case "$lib" in
					/*)
						cp "$lib" "Contents/Frameworks/$lib_bn"
					;;
					*)
						cp "$fw_basepath/$lib" "Contents/Frameworks/$lib_bn"
					;;
				esac
				install_name_tool \
					-id "@executable_path/../Frameworks/$lib_bn" \
					"Contents/Frameworks/$lib_bn" || exit 1
				todo="$todo Contents/Frameworks/$lib_bn"
			fi
			install_name_tool -change \
				"$lib" \
				"@executable_path/../Frameworks/$lib_bn" \
				"$todo_obj" || exit 1
		done
	done
done
echo "Bundle completed"
