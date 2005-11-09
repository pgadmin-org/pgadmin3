%define wxGTK_version 2.6.0
%define pg_version 8.0.3
%define withwxconfig wx-config
%define desktop_vendor pgadmin

Summary: Graphical client for PostgreSQL
Name: pgadmin3
Version: 1.2.2
Release: 3
License: Artistic
Group: Applications/Databases
Source: pgadmin3-%{version}.tar.gz
Source1: wxGTK-%{wxGTK_version}.tar.bz2
Source2: postgresql-%{pg_version}.tar.bz2
URL: http://www.pgadmin.org/
Packager: pgAdmin project <pgadmin-hackers@postgresql.org>
BuildRoot: %{_tmppath}/%{name}-root
BuildPrereq: pango-devel, glib2-devel, gtk2-devel, freetype-devel, openssl-devel
BuildPrereq: zlib-devel >= 1.0.4, krb5-devel, gettext
BuildPrereq: perl, glibc-devel, bison, flex
BuildPrereq: /usr/bin/desktop-file-install
Requires: pango, glib2, gtk2, freetype, openssl

%description
pgAdmin III is a powerful visual administration and development
platform for the PostgreSQL database.

%prep
%setup -q
tar xfj %{SOURCE1}
tar xfj %{SOURCE2}

%build
export CFLAGS="%{optflags}"
export CXXFLAGS="%{optflags}"
cd postgresql-%{pg_version}
./configure --prefix=%{_builddir}/%{name}-%{version}/tmp --with-openssl && make install
cd %{_builddir}/%{name}-%{version}/wxGTK-%{wxGTK_version}
mkdir tmp
./configure --prefix=%{_builddir}/%{name}-%{version}/tmp --with-gtk --enable-gtk2 \
    --enable-unicode --disable-shared --enable-mimetype=no && \
    make install && \
    cd contrib/src/stc && \
    make install
cd %{_builddir}/%{name}-%{version}
%configure --enable-static --with-wx=%{_builddir}/%{name}-%{version}/tmp --with-pgsql=%{_builddir}/%{name}-%{version}/tmp
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
* Wed Dec 1 2004 Troels Arvin <troels@arvin.dk>
- Include wxGTK2 and postgresql sources do minimize build dependencies.

* Mon Nov 29 2004 Devrim GUNDUZ <devrim@gunduz.org>
- 1.2.0 Gold

* Wed Nov 17 2004 Devrim GUNDUZ <devrim@gunduz.org>
- 1.2.0 RC2 

* Mon Nov 15 2004 Devrim GUNDUZ <devrim@gunduz.org>
- 1.2.0 RC1 rebuilt
- Fixed spec file so that beta and rc tags will be considered. (Note: AFAICS my previous rpms were broken :( )

* Thu Nov 4 2004 Devrim GUNDUZ <devrim@gunduz.org>
- 1.2.0 RC1

* Mon Jun 10 2003 Jean-Michel POURE <pgadmin-hackers@postgresql.org>
- Initial build


