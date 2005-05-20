%define major_version 1.2.2
%define minor_version %(date +%Y%m%d)
%define wxGTK2_version 2.6.0
%define withwxconfig wxgtk2ud-2.6-config
%define desktop_vendor pgadmin
%define debug_package %{nil}
%define __os_install_post /usr/lib/rpm/brp-compress

Summary: Graphical client for PostgreSQL.
Name: pgadmin3
Version: %{major_version}
Release: %{minor_version}
License: Artistic
Group: Applications/Databases
Source: pgadmin3-%{major_version}.tar.bz2
URL: http://www.pgadmin.org/
Packager: pgAdmin project <pgadmin-hackers@postgresql.org>
BuildRoot: %{_tmppath}/%{name}-root

BuildRequires: wxGTK2ud >= 2.6-%{wxGTK2_version}, wxGTK2ud-devel >= 2.6-%{wxGTK2_version}, wxGTK2ud-stc >= 2.6-%{wxGTK2_version}

%description
PostgreSQL Tools.

%prep
%setup -q

%build
export CFLAGS=""
export CXXFLAGS=""
%configure --enable-debug --enable-static --with-wx-config=%{withwxconfig}
make all

%install
rm -rf %{buildroot}
make install DESTDIR=%{buildroot}
#strip --strip-debug %{buildroot}/%{_bindir}/%{name}

cp -f ./src/include/images/elephant48.xpm %{buildroot}/%{_datadir}/%{name}/%{name}.xpm
cp -f ./pkg/%{name}.desktop %{buildroot}/%{_datadir}/%{name}/%{name}.desktop

mkdir -p %{buildroot}/%{_datadir}/applications

desktop-file-install --vendor %{desktop_vendor} --delete-original  --dir %{buildroot}/%{_datadir}/applications --add-category X-Red-Hat-Base --add-category Application --add-category Development %{buildroot}/%{_datadir}/%{name}/%{name}.desktop

%clean
rm -rf %{buildroot}

%files
%defattr(-, root, root)
%{_bindir}/%{name}
%{_datadir}/%{name}
%{_datadir}/applications

%changelog
* Mon Jun 10 2003 Jean-Michel POURE <pgadmin-hackers@postgresql.org>
- Initial build

