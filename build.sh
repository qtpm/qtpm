#!/bin/sh
set -e
go-bindata -pkg qtpm -nometadata -o qtpmlib/bindata.go templates licenses resources
pushd qtpmlib >> /dev/null
go fmt
popd >> /dev/null
go fmt
go build -ldflags="-w -s" -o qtpm

rm -rf ./workbench*

echo "application test"
mkdir workbench1
pushd workbench1 >> /dev/null
../qtpm init app
../qtpm build
../qtpm pack
popd >> /dev/null

echo "library test"
mkdir workbench2
pushd workbench2 >> /dev/null
../qtpm init lib
../qtpm build
../qtpm test
popd >> /dev/null

#echo "install library from git test"
#mkdir workbench3
#pushd workbench3 >> /dev/null
#../qtpm get github.c../qtpm/QtCBOR
#popd >> /dev/null
#
#echo "install library from git test and save to package"
#mkdir workbench4
#pushd workbench4 >> /dev/null
#../qtpm init app >> /dev/null
#../qtpm get github.c../qtpm/QtCBOR
#popd >> /dev/null
#
#echo "installing library is ignored when the repository exist"
#mkdir workbench5
#pushd workbench5 >> /dev/null
#echo "prepare"
#../qtpm get github.c../qtpm/QtCBOR
#echo "ignore"
#../qtpm get github.c../qtpm/QtCBOR
#popd >> /dev/null
#
#echo "force installing library when the repository exist"
#mkdir workbench6
#pushd workbench6 >> /dev/null
#echo "prepare"
#../qtpm get github.c../qtpm/QtCBOR
#echo "force install"
#../qtpm get -f github.c../qtpm/QtCBOR
#popd >> /dev/null

echo "nested required modules"
mkdir -p workbench7/vendor
cp -r test/github.com___shibukawa___package1 workbench7/vendor/
cp -r test/github.com___shibukawa___package2 workbench7/vendor/
pushd workbench7 >> /dev/null
../qtpm init app
cp ../test/package7.cpp src/main.cpp
../qtpm get github.com/shibukawa/package2  # pakcage2 depends on package1
../qtpm build
popd >> /dev/null

echo "conditional build test"
mkdir workbench8
pushd workbench8 >> /dev/null
../qtpm init app
cp ../test/package8_windows.cpp src
cp ../test/package8_linux.cpp src
cp ../test/package8_darwin.cpp src
cp ../test/package8_main.cpp src/main.cpp
mkdir -p qtresources/translations/
cp ../test/workbench8_ja.ts translations/
../qtpm build
popd >> /dev/null

#echo "conditional build test"
#mkdir workbench9
#pushd workbench9 >> /dev/null
#../qtpm init lib
#cp ../test/package9.toml qtpackage.toml
#mkdir src/extra
#cp ../test/package9_extra_header.h src/extra
#../qtpm build
#popd >> /dev/null
