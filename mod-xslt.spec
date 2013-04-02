# RPM Spec file for mod_xslt2

%define name      mod-xslt
%define summary   mod-xslt module
%define license   GPL2
%define version   1.4.1
%define release   1
%define group     Web/Applications
%define source    %{name}-%{version}.tar.gz
%define packager  ccontavalli@inscatolati.net
%define buildroot %{_builddir}/%{name}-root

Name:      %{name}
Version:   %{version}
Release:   %{release}
Packager:  %{packager}
Summary:   %{summary}
License:  %{license}
Group:     %{group}
Source:   %{source}
Prefix:    %{_prefix}
Buildroot: %{buildroot}
BuildRequires: libxslt-devel, libxml2-devel, httpd-devel

%description
mod_xslt is a simple Apache module to serve XML based content.

%package apache
Summary:        Apache httpd mod_xslt
Group:          Web/Applications
Requires:       %{name} = %{version}-%{release}, httpd > 2.2

%description apache
Apache httpd mod_xslt module.

%package devel
Summary:   Development headers for mod_xslt
Group:     Development/Libraries
Requires:  %{name} = %{version}-%{release}

%description devel
Development headers for mod_xslt.

%prep
%setup -q
%build
%configure \
   --with-sapi=apache2 \
   --with-apr-config=%{_bindir}/apr-1-config \
   --with-apu-config=%{_bindir}/apu-1-config \
   --with-apxs=%{_sbindir}/apxs
%{__make}

%install
rm -rf $RPM_BUILD_ROOT
%{__make} install DESTDIR=$RPM_BUILD_ROOT

%clean
rm -rf $RPM_BUILD_ROOT

%post
/sbin/ldconfig

%postun
/sbin/ldconfig

%files
%defattr(644,root,root,755)
%attr(755,root,root) %{_libdir}/libmodxslt1.so.*.*.*
%attr(755,root,root) %{_libdir}/libmodxslt1.so.*
%attr(755,root,root) %{_libdir}/libmodxslt1.so

%files apache
%attr(755,root,root) %{_libdir}/httpd/modules/*.so

%files devel
%defattr(644,root,root,755)
%attr(755,root,root) %{_bindir}/*
%{_libdir}/*.la
%{_includedir}/modxslt1
%defattr(644,root,root,755)
%{_libdir}/*.a
%{_mandir}/man1/modxslt-config.1*
%{_mandir}/man1/modxslt-parse.1*
%{_mandir}/man1/modxslt-perror.1*

