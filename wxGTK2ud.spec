%define minor_version 20030607
%define major_version 2.5

Summary: The GTK+ 2 port of the wxWindows library
Name: wxGTK2ud
Version: %{major_version}
Release: %{minor_version}
Copyright: wxWindows Licence
Group: X11/Libraries
Source: %{name}-%{version}.tar.bz2
URL: http://www.wxwindows.org
Packager: pgAdmin project <pgadmin-hackers@postgresql.org>
BuildRoot: %{_tmppath}/%{name}-root

# The package advertises only for libwx_gtk-%{ver2}.a
# Therefore, there is no incompatibility with wxGTK 2.4

Provides: libwx_gtk2ud-%{version}.a

%description
wxWindows is a free C++ library for cross-platform GUI development.
With wxWindows, you can create applications for different GUIs (GTK+,
Motif/LessTif, MS Windows, Mac) from the same source code. These packages
are provided with Unicode and debug features and linked agains GTK2 staticly.
You can use them for the development of pgAdmin3. They will not conflict with
existing GTK 2.4 libraries.

%package devel
Summary: The GTK+ 2 port of the wxWindows library
Group: X11/Libraries
Requires: %{name} = %{version}-%{release}

%description devel
Header files for wxGTK, the GTK+ 2 port of the wxWindows library.

%package xrc
Summary: The XML-based resource system for the wxWindows library.
Group: System Environment/Libraries
Requires: %{name} = %{version}-%{release}

%description xrc
The XML-based resource system, known as XRC, allows user interface
elements such as dialogs, menu bars and toolbars, to be stored in
text files and loaded into the application at run-time.

%package stc
Summary: Styled text control.

Group: System Environment/Libraries
Requires: %{name} = %{version}-%{release}

%description stc
Styled text control based on the Scintillia project http://www.scintilla.org/.

%prep
%setup -q

%build
%configure --with-gtk --enable-gtk2 --enable-unicode --enable-debug --disable-shared
make %{?_smp_mflags}

pushd contrib/src/xrc
  make %{?_smp_mflags}
popd
pushd contrib/utils/wxrc
  make %{?_smp_mflags}
popd

pushd contrib/src/stc
  make %{?_smp_mflags}
popd

%install
rm -rf %{buildroot}
%makeinstall

pushd contrib/src/xrc
%makeinstall
popd
pushd contrib/utils/wxrc
%makeinstall
popd

pushd contrib/src/stc
%makeinstall
popd

%clean
rm -rf %{buildroot}

%post -p /sbin/ldconfig
%postun -p /sbin/ldconfig

%post xrc -p /sbin/ldconfig
%postun xrc -p /sbin/ldconfig

%post stc -p /sbin/ldconfig
%postun stc -p /sbin/ldconfig

%files
%defattr(-,root,root)
%{_libdir}/libwx_gtk2ud-%{version}.a
%{_datadir}/wx

%files devel
%defattr(-,root,root)
%{_includedir}/wx
%{_libdir}/wx
%{_bindir}/wxgtk2ud-%{version}-config
%{_datadir}/aclocal/*.m4

%files xrc
%defattr(-, root, root)
%{_libdir}/libwx_gtk2ud_xrc-%{version}.a
%{_libdir}/libwx_gtk2ud_xrc-%{version}.a
/bin/wxrc

%files stc
%defattr(-, root, root)
%{_libdir}/libwx_gtk2ud_stc-%{version}.a

%changelog
* Mon Jun 10 2003 Jean-Michel POURE <jm.poure@freesurf.fr>
- Initial build.
