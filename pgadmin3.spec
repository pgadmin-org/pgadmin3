%define major_version      0.1.1
%define minor_version      %(date +%Y%m%d)
%define wxGTK2_version     20030607
%define with_wx_config     wxgtk2ud-2.5-config
%define with_pgsql         /usr
%define with_pgsql_include /usr/include

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

#
# German localisation
#
%package i18N-de_DE
Summary: pgAdmin3 German localisation files.
Group: Applications/Databases
Requires: %{name} = %{version}-%{release}

%description i18N-de_DE
pgAdmin3 German translation by Andreas Pflug <andreas.pflug@web.de>.

#
# French localisation
#
%package i18N-fr_FR
Summary: pgAdmin3 French localisation files.
Group: Applications/Databases
Requires: %{name} = %{version}-%{release}

%description i18N-fr_FR
pgAdmin3 French translation by Jean-Michel POURE <jm.poure@freesurf.fr>.

#
# Japanese localisation
#
%package i18N-ja_JP
Summary: pgAdmin3 French localisation files.
Group: Applications/Databases
Requires: %{name} = %{version}-%{release}

%description i18N-ja_JP
pgAdmin3 japanese translation by Hiroshi Saito <saito@inetrt.skcapi.co.jp>.


%prep
%setup -q

%build
%configure --enable-debug --enable-static --with-wx-config=%{with_wx_config} --with-pgsql=%{with_pgsql} --with-pqsql-include=%{with_pgsql_include}
make all

%install
rm -rf %{buildroot}
make install DESTDIR=%{buildroot}

%clean
rm -rf %{buildroot}

%files
%defattr(-, root, root)
%{with_pgsql}/bin
%{with_pgsql}/share/pgadmin3/tips.txt
%{with_pgsql}/share/pgadmin3/docs
%{with_pgsql}/share/pgadmin3/ui/common

%files i18N-de_DE
%defattr(-, root, root)
%{with_pgsql}/share/pgadmin3/ui/de_DE

%files i18N-fr_FR
%defattr(-, root, root)
%{with_pgsql}/share/pgadmin3/ui/fr_FR

%files i18N-ja_JP
%defattr(-, root, root)
%{with_pgsql}/share/pgadmin3/ui/ja_JP

%changelog
* Mon Jun 10 2003 Jean-Michel POURE <jm.poure@freesurf.fr>
- Initial build

