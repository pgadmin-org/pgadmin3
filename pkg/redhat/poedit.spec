Summary:       Gettext catalogs editor
Name:          poedit
Version:       1.2.2
Release:       1
License:       MIT
Group:         Applications/Editors
Source:        poedit-%{version}.tar.gz
URL:           http://poedit.sourceforge.net
Packager:      Vaclav Slavik <vaclav.slavik@matfyz.cz>
Provides:      poedit
BuildRoot:     %{_tmppath}/%{name}-%{version}

%description
poEdit is cross-platform gettext catalogs (.po files) editor. It is built with
wxWindows toolkit and can run on Unix or Windows. It aims to provide convenient
way of editing gettext catalogs. It features UTF-8 support, fuzzy and
untranslated records highlighting, whitespaces highlighting, references
browser, headers editing and can be used to create new catalogs or update
existing catalogs from source code by single click.

%prep
%setup -q

%build
%configure --enable-semistatic --enable-debug
make

%install
rm -rf %{buildroot}
%makeinstall GNOME_DATA_DIR=%{buildroot}/usr/share \
             KDE_DATA_DIR=%{buildroot}/usr/share

(cd %{buildroot}
mkdir -p ./%{_libdir}/menu
cat > ./%{_libdir}/menu/poedit <<EOF 
?package(%{name}): \
	command="%{_bindir}/poedit"\\
	needs="X11"\\
	section="Applications/Development/Tools"\\
	icon="poedit.xpm"\\
	mimetypes="application/x-po;application/x-gettext"\\
	title="poEdit"\\
	longtitle="poEdit Gettext Catalogs Editor"
EOF
)


%find_lang poedit
%find_lang poedit-wxstd
cat poedit-wxstd.lang >>poedit.lang

%clean
rm -Rf %{buildroot}

%post
# This is done on Mandrake to update its menus:
if [ -x /usr/bin/update-menus ]; then /usr/bin/update-menus || true ; fi

%postun
# This is done on Mandrake to update its menus:
if [ "$1" = "0" -a -x /usr/bin/update-menus ]; then /usr/bin/update-menus || true ; fi


%files -f poedit.lang
%defattr(-,root,root)
%doc NEWS LICENSE README AUTHORS

%dir %{_datadir}/poedit
%{_datadir}/poedit/*
%lang(hr) %{_datadir}/poedit/help-hr.zip
%{_bindir}/poedit
%{_mandir}/*/*
%{_datadir}/gnome/apps/Development/*
%{_datadir}/mime-info/poedit.*
%{_datadir}/pixmaps/*
%{_libdir}/menu/*
