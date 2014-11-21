#!/bin/sh
bundle="$1"

if ! test -d "$bundle" ; then
	echo "$bundle is no bundle!" >&2
	exit 1
fi

test -d "$bundle/Contents/Frameworks" || mkdir -p "$bundle/Contents/Frameworks" || exit 1

function CompleteSingleApp() {
	local bundle=$1 tag=$(basename "$1") todo todo_old fw_relpath lib lib_bn nested_app na_relpath

	echo "Completing app: $bundle"
	pushd "$bundle" > /dev/null

	#We skip nested apps here - those are treated specially
	todo=$(file `find ./ -perm +0111 ! -type d ! -path "*.app/*" ! -name "*.app"` | grep -E "Mach-O (i386 )?executable" | awk -F ':| ' '{ORS=" "; print $1}' | uniq)

	echo "App: $tag: Found executables: $todo"
	while test "$todo" != ""; do
		todo_old=$todo ;
		todo="" ;
		for todo_obj in $todo_old; do
			echo "App: $tag: Post-processing: $todo_obj"

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
                                        target_file=""
					target_path=""
					echo "App: $tag: Adding symlink: $lib_bn (because of: $todo_obj)"
					cp -R "$lib" "Contents/Frameworks/$lib_bn"
					if ! test -L "Contents/Frameworks/$lib_bn"; then
						chmod 755 "Contents/Frameworks/$lib_bn"
					else
						target_file=$(readlink "$lib")
						target_path=$(dirname "$lib")/$target_file
					        echo "App: $tag: Adding symlink target: $target_path"
						cp "$target_path" "Contents/Frameworks/$target_file"
						chmod 755 "Contents/Frameworks/$target_file"
					fi
					echo "Rewriting ID in Contents/Frameworks/$lib_bn to $lib_bn"
                                        install_name_tool \
                                                -id "$lib_bn" \
                                                "Contents/Frameworks/$lib_bn" || exit 1
					todo="$todo ./Contents/Frameworks/$lib_bn"
				fi
				echo "Rewriting library $lib to @executable_path/$fw_relpath/$lib_bn in $todo_obj"
				install_name_tool -change \
					"$lib" \
					"@executable_path/$fw_relpath/$lib_bn" \
					"$todo_obj" || exit 1
                                install_name_tool -change \
                                        "$target_path" \
                                        "@executable_path/$fw_relpath/$target_file" \
                                        "$todo_obj" || exit 1
			done
		done
	done

	#We handle only one level here, because this is recursive anyway
	for nested_app in $(find ./ -type d -name "*.app" ! -path "*.app/*"); do
		echo "App: $tag: Post-processing nested app: $nested_app"
		
		if ! test -d "$nested_app/Contents/Frameworks"; then
			echo "App: "$(basename "$nested_app")": Symlinking Frameworks-folder to parent $bundle"
			na_relpath=$(echo "$nested_app" |\
				sed -n 's|^\(\.//*\)\(\([^/][^/]*/\)*\)[^/][^/]*$|\2|gp' | \
				sed -n 's|[^/][^/]*/|../|gp' \
			)"Contents/Frameworks"
			ln -s "../../$na_relpath" "$nested_app/Contents/Frameworks"
		fi
		
		CompleteSingleApp "$nested_app"
	done

	echo "App completed: $bundle"
	popd > /dev/null
}

CompleteSingleApp "$bundle"
