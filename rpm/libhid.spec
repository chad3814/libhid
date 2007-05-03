Summary: A userspace USB HID access library
Name: libhid
Version: 0.2.16
Release: 1
Source0: %{name}-%{version}.tar.gz
#Patch0: libhid-0.2.10-am_config_header.patch
#Patch1: libhid-0.2.10-doc_makefile_am.patch
#Patch2: libhid-0.2.10-src_makefile_am.patch
#Patch3: libhid-0.2.10-m4_doxygen.patch
#Patch4: libhid-0.2.10-docbook_xsl_location-redhat.patch
License: GPL
Group: System Environment/Libraries
BuildRoot: %{_tmppath}/%{name}-%{version}-root
URL: http://libhid.alioth.debian.org
Requires: libusb
BuildRequires: libusb-devel
BuildRequires: libtool
BuildRequires: pkgconfig
#BuildRequires: swig
#BuildRequires: docbook-utils
##BuildRequires: doxygen
ExcludeArch: s390 s390x

%description
libhid provides a generic and flexible way to access and interact with USB
HID devices, much like libusb does for plain USB devices. It is based on
libusb, thus it requires no HID support in the kernel and provides means to
take control over a device even if the kernel governs it.

%package devel
Summary: Development files for libhid.
Group: Development/Libraries
Requires: %{name} = %{version}

%description devel
This package provides the development files and static library for libhid.
You need this if you want to develop an application with libhid.

%prep
# %setup -q # -n %{name}-%{version}.orig
%setup -q
#%patch0 -p1
#%patch1 -p1
#%patch2 -p1
#%patch3 -p1
#%patch4 -p1
#libtoolize --copy --force
#aclocal -I m4
#autoheader
#automake --add-missing
#autoconf

%build
%configure --disable-swig --without-doxygen     # disable swig and doxygen  for now
make CFLAGS="$CFLAGS"

%install
rm -rf $RPM_BUILD_ROOT
%makeinstall

%clean
rm -rf $RPM_BUILD_ROOT

%post -p /sbin/ldconfig
%postun -p /sbin/ldconfig


%files
%defattr(-,root,root)
%{_libdir}/*.so.*
%{_bindir}/libhid-detach-device
%{_libdir}/pkgconfig/libhid.pc
%{_mandir}/man1/libhid-detach-device.1.gz


%files devel
%defattr(-,root,root)
%doc AUTHORS README NEWS ChangeLog
%{_includedir}/*
%{_libdir}/*.so
%{_libdir}/*.a
%{_libdir}/*.la


%changelog
* Thu May 03 2007 Charles Lepple <clepple+libhid@ghz.cc> 0.2.16-1
- Built on Fedora Core 6
* Sat Jan 15 2005 Jason Watson <jason.watson@agrios.net> 0.2.10-1
- Initial RPM build

