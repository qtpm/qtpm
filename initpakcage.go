package main

import (
	"log"
	"os"
	"path/filepath"
	"strings"
)

func prepareProject(name, license string) (*PackageConfig, string) {
	dir, err := filepath.Abs(".")
	if err != nil {
		log.Fatalln("Can't get directory name")
	}

	licenseKey, licenseName, err := NormalizeLicense(license)
	if err != nil {
		log.Fatalln(err)
	}

	config := &PackageConfig{
		Name:    name,
		Author:  UserName(),
		License: licenseName,
		Dir:     dir,
	}
	WriteLicense(dir, licenseKey)
	return config, dir
}

func initDirs(workDir string, extraDirs ...string) {
	dirs := []string{"src", "src/private", "resource", "test", "build", "vendor", "doc", "html"}
	dirs = append(dirs, extraDirs...)
	for _, dir := range dirs {
		os.MkdirAll(filepath.Join(workDir, dir), 0744)
	}
}

func InitLibrary(name, license string) {
	packageName, parentName := ParseName(name)
	config, dir := prepareProject(packageName, license)
	initDirs(dir)
	config.Save()
	variable := &SourceVariable{
		Target: packageName,
		Parent: parentName,
	}
	AddClass(".", packageName, true)
	AddTest(".", packageName)
	WriteTemplate(".", "src", strings.ToLower(packageName)+"_global.h", "libglobal.h", variable, false)
	WriteTemplate(".", "", ".gitignore", "dotgitignore", variable, false)
	WriteTemplate(".", "", "CMakeExtra.txt", "CMakeExtra.txt", variable, false)
	WriteTemplate(".", "", "CMakeExtra.txt", "CMakeExtra.txt", variable, false)
}

func InitApplication(name, license string) {
	packageName, _ := ParseName(name)
	config, dir := prepareProject(packageName, license)
	initDirs(dir, "resource")
	config.QtModules = []string{"Widgets"}
	config.Save()

	variable := &SourceVariable{
		Target:    packageName,
		QtModules: []string{"Widgets"},
	}
	WriteTemplate(".", "src", "main.cpp", "main.cpp", variable, false)
	WriteTemplate(".", "", ".gitignore", "dotgitignore", variable, false)
	WriteTemplate(".", "", "CMakeExtra.txt", "CMakeExtra.txt", variable, false)
}
