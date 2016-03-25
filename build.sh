#!/bin/sh
set -e
go-bindata -pkg qtpm -nometadata templates licenses resources
go fmt
pushd qtpm >> /dev/null
go fmt
go build -ldflags="-w -s" -o ../qtpmcmd
popd >> /dev/null

rm -rf ./workbench*

echo "application test"
mkdir workbench1
pushd workbench1 >> /dev/null
../qtpmcmd init app
../qtpmcmd build
../qtpmcmd pack
popd >> /dev/null

echo "library test"
mkdir workbench2
pushd workbench2 >> /dev/null
../qtpmcmd init lib
../qtpmcmd build
../qtpmcmd test
popd >> /dev/null

#echo "install library from git test"
#mkdir workbench3
#pushd workbench3 >> /dev/null
#../qtpmcmd get github.c../qtpmcmd/QtCBOR
#popd >> /dev/null
#
#echo "install library from git test and save to package"
#mkdir workbench4
#pushd workbench4 >> /dev/null
#../qtpmcmd init app >> /dev/null
#../qtpmcmd get github.c../qtpmcmd/QtCBOR
#popd >> /dev/null
#
#echo "installing library is ignored when the repository exist"
#mkdir workbench5
#pushd workbench5 >> /dev/null
#echo "prepare"
#../qtpmcmd get github.c../qtpmcmd/QtCBOR
#echo "ignore"
#../qtpmcmd get github.c../qtpmcmd/QtCBOR
#popd >> /dev/null
#
#echo "force installing library when the repository exist"
#mkdir workbench6
#pushd workbench6 >> /dev/null
#echo "prepare"
#../qtpmcmd get github.c../qtpmcmd/QtCBOR
#echo "force install"
#../qtpmcmd get -f github.c../qtpmcmd/QtCBOR
#popd >> /dev/null

echo "nested required modules"
mkdir -p workbench7/vendor
cp -r test/github.com___shibukawa___package1 workbench7/vendor/
cp -r test/github.com___shibukawa___package2 workbench7/vendor/
pushd workbench7 >> /dev/null
../qtpmcmd init app
cp ../test/package7.cpp src/main.cpp
../qtpmcmd get github.com/shibukawa/package2  # pakcage2 depends on package1
../qtpmcmd build
popd >> /dev/null

echo "conditional build test"
mkdir workbench8
pushd workbench8 >> /dev/null
../qtpmcmd init app
cp ../test/package8_windows.cpp src
cp ../test/package8_linux.cpp src
cp ../test/package8_darwin.cpp src
cp ../test/package8_main.cpp src/main.cpp
mkdir -p qtresources/translations/
cp ../test/workbench8_ja.ts translations/
../qtpmcmd build
popd >> /dev/null

#echo "conditional build test"
#mkdir workbench9
#pushd workbench9 >> /dev/null
#../qtpmcmd init lib
#cp ../test/package9.toml qtpackage.toml
#mkdir src/extra
#cp ../test/package9_extra_header.h src/extra
#../qtpmcmd build
#popd >> /dev/null
