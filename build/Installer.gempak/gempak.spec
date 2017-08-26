#
# Unidata GEMPAK Spec File
# 
# Nov 22, 2016  mjames  Created
# Aug 24, 2017  mjames  7.4.0 docker rpm builds w traviscl
#
%define __prelink_undo_cmd %{nil}
%define gem_home /home/gempak/GEMPAK7
%define prefix /home/gempak
%define version 7.4.0
Name: gempak
Summary: Unidata GEMPAK
Version: %{version}
Release: 1%{?dist}
Prefix: %{prefix}
Group: GEMPAK
BuildRoot: /tmp
URL: N/A
License: N/A
Distribution: N/A
Vendor: Unidata
Packager: Michael James
BuildRequires: libX11-devel, libXt-devel, libXext, libXp-devel
BuildRequires: libXft-devel, libXtst-devel, xorg-x11-xbitmaps
BuildRequires: flex, byacc, openmotif-devel
Requires: openmotif, libX11, libXt, libXext, libXp, libXft, libXtst,  xorg-x11-xbitmaps
AutoReq: no
provides: gempak

%description
Unidata GEMPAK Distribution

%prep
mkdir -p %{prefix}
pushd %{prefix}
tar -xvzf /tmp/rpmbuild/SOURCES/gempak-%{version}.tar.gz -C %{prefix}/ >& /dev/null

%build

%install
# create build root directory
pushd %{gem_home}
export NAWIPS=`pwd`
. build/Installer.gempak/Gemenviron.profile
pwd
ls -al
. source_python.sh

make extlibs >& /dev/null
make gempak >& /dev/null
make install >& /dev/null
make programs_gf >& /dev/null
make programs_nc >& /dev/null
make clean >& /dev/null

mkdir -p ${RPM_BUILD_ROOT}/home/gempak/
cd ..
mv GEMPAK7 ${RPM_BUILD_ROOT}/home/gempak/
cd ${RPM_BUILD_ROOT}/home/gempak/GEMPAK7
rm -rf extlibs config .gitignore .travis.yml build

%pre

%post
ln -s %{prefix}/GEMPAK7 %{prefix}/NAWIPS

%postun

%clean

%files
%defattr(-,gempak,-,-)
%{prefix}/GEMPAK7
