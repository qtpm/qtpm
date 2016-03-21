package qtpm

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
		Description:      "Write your project description here",
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
	dirs := []string{"src", "src/private", "translations", "qtresources", "test", "vendor", "doc", "html"}
	dirs = append(dirs, extraDirs...)
	for _, dir := range dirs {
		os.MkdirAll(filepath.Join(workDir, dir), 0755)
	}
}

func InitLibrary(name, license string) {
	_, packageName, _ := ParseName(name)
	config, dir := prepareProject(packageName, license)
	initDirs(dir, "examples", "dest/release", "dest/debug")
	config.Save()
	variable := &SourceVariable{
		config: config,
		Target: packageName,
	}
	AddClass(config, packageName, true)
	AddTest(config, packageName)
	WriteTemplate(".", "src", strings.ToLower(packageName)+"_global.h", "libglobal.h", variable, false)
	WriteTemplate(".", "examples", "example.cpp", "main.cpp", variable, false)
	WriteTemplate(".", "", ".gitignore", "dotgitignore", variable, false)
	WriteTemplate(".", "", ".clang-format", "dotclang-format", variable, false)
	WriteTemplate(".", "", "CMakeExtra.txt", "CMakeExtra.txt", variable, false)
	WriteTemplate(".", "", "README.rst", "READMELib.rst", variable, false)
	Touch(false)
}

func InitApplication(name, license string) {
	_, packageName, _ := ParseName(name)
	config, dir := prepareProject(packageName, license)
	initDirs(dir, "Resource")
	config.Save()

	variable := &SourceVariable{
		config:    config,
		Target:    packageName,
		QtModules: []string{"Widgets"},
	}
	WriteTemplate(".", "src", "main.cpp", "main.cpp", variable, false)
	WriteTemplate(".", "", ".gitignore", "dotgitignore", variable, false)
	WriteTemplate(".", "", ".clang-format", "dotclang-format", variable, false)
	WriteTemplate(".", "", "CMakeExtra.txt", "CMakeExtra.txt", variable, false)
	WriteTemplate(".", "", "README.rst", "READMEApp.rst", variable, false)
	Touch(false)
}
