package main

import (
	"log"
	"os"
	"path/filepath"
	"strings"
	"time"
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
		Name:             name,
		Author:           UserName(),
		License:          licenseName,
		Requires:         make([]string, 0),
		QtModules:        []string{"Core", "Gui", "Widgets"},
		Dir:              dir,
		Version:          []int{0, 1, 0},
		ProjectStartYear: time.Now().Year(),
	}
	WriteLicense(dir, licenseKey)
	return config, dir
}

func initDirs(workDir string, extraDirs ...string) {
	dirs := []string{"src", "src/private", "resource", "test", "vendor", "doc", "html"}
	dirs = append(dirs, extraDirs...)
	for _, dir := range dirs {
		os.MkdirAll(filepath.Join(workDir, dir), 0755)
	}
}

func InitLibrary(name, license string) {
	packageName, parentName := ParseName(name)
	config, dir := prepareProject(packageName, license)
	initDirs(dir, "examples")
	config.Save()
	variable := &SourceVariable{
		config: config,
		Target: packageName,
		Parent: parentName,
	}
	AddClass(".", packageName, true)
	AddTest(".", packageName)
	WriteTemplate(".", "src", strings.ToLower(packageName)+"_global.h", "libglobal.h", variable, false)
	WriteTemplate(".", "examples", "example.cpp", "main.cpp", variable, false)
	WriteTemplate(".", "", ".gitignore", "dotgitignore", variable, false)
	WriteTemplate(".", "", "CMakeExtra.txt", "CMakeExtra.txt", variable, false)
	WriteTemplate(".", "", "CMakeExtra.txt", "CMakeExtra.txt", variable, false)
	WriteTemplate(".", "", "README.rst", "READMELib.rst", variable, false)
}

func InitApplication(name, license string) {
	packageName, _ := ParseName(name)
	config, dir := prepareProject(packageName, license)
	initDirs(dir)
	config.Save()

	variable := &SourceVariable{
		config:    config,
		Target:    packageName,
		QtModules: []string{"Widgets"},
	}
	WriteTemplate(".", "src", "main.cpp", "main.cpp", variable, false)
	WriteTemplate(".", "", ".gitignore", "dotgitignore", variable, false)
	WriteTemplate(".", "", "CMakeExtra.txt", "CMakeExtra.txt", variable, false)
	WriteTemplate(".", "", "README.rst", "READMEApp.rst", variable, false)
}
