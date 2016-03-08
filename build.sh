#!/bin/sh
set -e
go-bindata templates licenses
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
#
