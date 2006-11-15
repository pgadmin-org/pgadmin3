#!/bin/sh
bundle="$1"

if ! test -d "$bundle" ; then
	echo "$bundle is no bundle!" >&2
	exit 1
fi

test -d "$bundle/Contents/Frameworks" || mkdir -p "$bundle/Contents/Frameworks" || exit 1

echo "Completing bundle: $bundle"

cd "$bundle"

todo=$(file `find ./ -perm +0111 ! -type d` | grep "Mach-O executable" | awk -F: -F\  '{print $1}' | uniq)

echo "Found executables: $todo"

while test "$todo" != ""; do
	todo_old=$todo ;
	todo="" ;
	for todo_obj in $todo_old; do
		echo "Post-processing: $todo_obj"

		#Figure out the relative path from todo_obj to Contents/Frameworks
		fw_relpath=$(echo "$todo_obj" |\
			sed -n 's|^\(\.//*\)\(\([^/][^/]*/\)*\)[^/][^/]*$|\2|gp' | \
			sed -n 's|[^/][^/]*/|../|gp' \
		)"Contents/Frameworks"

		#Find all libraries $todo_obj depends on, but skip system libraries
		for lib in $(
			otool -L $todo_obj | \
			sed -n 's|^.*[[:space:]]\([^[:space:]]*\.dylib\).*$|\1|p' | \
			egrep -v '^(/usr/lib)|(/System)|@executable_path@' \
		); do
			lib_bn="$(basename "$lib")" ;
			if ! test -f "Contents/Frameworks/$lib_bn"; then
				echo "Adding library: $lib_bn (because of: $todo_obj)"
				cp "$lib" "Contents/Frameworks/$lib_bn"
				chmod 755 "Contents/Frameworks/$lib_bn"
				install_name_tool \
					-id "$lib_bn" \
					"Contents/Frameworks/$lib_bn" || exit 1
				todo="$todo .//Contents/Frameworks/$lib_bn"
			fi
			install_name_tool -change \
				"$lib" \
				"@executable_path/$fw_relpath/$lib_bn" \
				"$todo_obj" || exit 1
		done
	done
done
echo "Bundle completed"
