%define major_version 0.9.3
%define minor_version %(date +%Y%m%d)
%define wxGTK2_version 20030831
%define withwxconfig wxgtk2ud-2.5-config

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

#
# When rebuilding, wxWindows 2.5 CVS version is needed.
# wxWindows 2.5 CVS (RPM and SRPMs) snapshots can be downloaded from
# http://www.pgadmin.org/pgadmin3/development.php page in the Snapshots section
#

BuildRequires: wxGTK2ud >= 2.5-%{wxGTK2_version}, wxGTK2ud-devel >= 2.5-%{wxGTK2_version}, wxGTK2ud-stc >= 2.5-%{wxGTK2_version}, wxGTK2ud-xrc >= 2.5-%{wxGTK2_version}

%description
PostgreSQL Tools.

%prep
%setup -q

%build
%configure --enable-debug --enable-static --with-wx-config=%{withwxconfig}
make all

%install
rm -rf %{buildroot}
make install DESTDIR=%{buildroot}
strip --strip-debug %{_bindir}/%{name}

%clean
rm -rf %{buildroot}

%files
%defattr(-, root, root)
%{_bindir}/%{name}
%{_datadir}/%{name}

%changelog
* Mon Jun 10 2003 Jean-Michel POURE <pgadmin-hackers@postgresql.org>
- Initial build

