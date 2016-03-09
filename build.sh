#!/bin/sh
set -e
go-bindata -nometadata templates licenses
go build -ldflags="-w -s"
go fmt

rm -rf ./workbench*

echo "application test"
mkdir workbench1
pushd workbench1
../qtpm init app
../qtpm build
popd

echo "library test"
mkdir workbench2
pushd workbench2
../qtpm init lib
../qtpm build
popd

#echo "install library from git test"
#mkdir workbench3
#pushd workbench3
#../qtpm get github.com/qtpm/QtCBOR
#popd
#
#echo "install library from git test and save to package"
#mkdir workbench4
#pushd workbench4
#../qtpm init app
#../qtpm get github.com/qtpm/QtCBOR
#popd
#
#echo "installing library is ignored when the repository exist"
#mkdir workbench5
#pushd workbench5
#echo "prepare"
#../qtpm get github.com/qtpm/QtCBOR
#echo "ignore"
#../qtpm get github.com/qtpm/QtCBOR
#popd
#
#echo "force installing library when the repository exist"
#mkdir workbench6
#pushd workbench6
#echo "prepare"
#../qtpm get github.com/qtpm/QtCBOR
#echo "force install"
#../qtpm get -f github.com/qtpm/QtCBOR
#popd

echo "nested required modules"
mkdir -p workbench7/vendor/github.com/shibukawa
cp -r test/package1 workbench7/vendor/github.com/shibukawa
cp -r test/package2 workbench7/vendor/github.com/shibukawa
pushd workbench7
../qtpm init app
cp ../test/package7.cpp src/main.cpp
../qtpm get github.com/shibukawa/package2  # pakcage2 depends on package1
../qtpm build
popd

echo "conditional build test"
mkdir workbench8
pushd workbench8
../qtpm init app
cp ../test/package8_windows.cpp src
cp ../test/package8_linux.cpp src
cp ../test/package8_darwin.cpp src
cp ../test/package8_main.cpp src/main.cpp
../qtpm build
popd
