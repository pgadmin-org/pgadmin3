%define major_version 0.8
%define minor_version %(date +%Y%m%d)
%define wxGTK2_version 20030707
%define withwxconfig wxgtk2ud-2.5-config
%define withpgsql /usr
%define withpgsqlinclude /usr/include

# Mandrake 91 & SuSE 8.2
# %define withpgsql_include --with-pgsql-include=/usr/include/pgsql

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
# When rebuilding, wxWindows 2.5 CVS version > 20030607 is needed.
# wxWindows 2.5 CVS (RPM and SRPMs) snapshots can be downloaded from
# http://www.pgadmin.org/snapshots in the directory of your distribution.
# 

BuildRequires: wxGTK2ud >= 2.5-%{wxGTK2_version}, wxGTK2ud-devel >= 2.5-%{wxGTK2_version}, wxGTK2ud-stc >= 2.5-%{wxGTK2_version}, wxGTK2ud-xrc >= 2.5-%{wxGTK2_version}

%description
pgAdmin3 is the graphical administration interface of PostgreSQL.

%prep
%setup -q

%build
%configure --enable-debug --enable-static --with-wx-config=%{withwxconfig} --with-pgsql=%{withpgsql} --with-pgsql-include=%{withpgsqlinclude}
make all

%install
rm -rf %{buildroot}
make install DESTDIR=%{buildroot}
strip --strip-debug %{buildroot}/usr/bin/pgadmin3

%clean
rm -rf %{buildroot}

%files
%defattr(-, root, root)
%{withpgsql}/bin
%{withpgsql}/share

%changelog
* Mon Jun 10 2003 Jean-Michel POURE <jm.poure@freesurf.fr>
- Initial build

