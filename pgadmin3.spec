%define major_version      0.1.1
%define minor_version      %(date +%Y%m%d)
%define wxGTK2_version     20030607
%define with_wx_config     --with-wx-config=wxgtk2ud-2.5-config
%define with_pgsql         --with-pgsql=/usr
%define with_pgsql_include --with-pgsql-include=/usr/include
%define prefix             /usr/local/pgadmin3

# Mandrake 91
# %define with_pgsql_include --with-pgsql-include=/usr/include/pgsql

Summary: Graphical client for PostgreSQL.
Name: pgadmin3
Version: %{major_version}
Release: %{minor_version}
License: Artistic
Group: Applications/Databases
Source: pgadmin3-%{version}.tar.bz2
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
%configure --enable-debug --enable-static --prefix=%{prefix} %{with_wx_config} %{with_pgsql} %{with_pgsql_include}
make all

%install
rm -rf %{buildroot}
make install DESTDIR=%{buildroot}

%clean
rm -rf %{buildroot}

%files
%defattr(-, root, root)
%{prefix}

%changelog
* Mon Jun 10 2003 Jean-Michel POURE <jm.poure@freesurf.fr>
- Initial build

