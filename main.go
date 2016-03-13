package main

import (
	"gopkg.in/alecthomas/kingpin.v2"
	"log"
	"os"
)

var (
	app               = kingpin.New("qtpm", "Package Manager fot Qt")
	verbose           = app.Flag("verbose", "Set verbose mode").Short('v').Bool()
	buildCommand      = app.Command("build", "Build program")
	buildTypeFlag     = buildCommand.Arg("build type", "release/debug").Default("debug").Enum("debug", "release")
	refreshBuildFlag  = buildCommand.Flag("refresh", "Refresh cache").Short('r').Bool()
	packCommand       = app.Command("pack", "Create installer")
	packTypeFlag      = packCommand.Arg("build type", "release/debug").Default("release").Enum("debug", "release")
	cleanCommand      = app.Command("clean", "Clean temp files")
	getCommand        = app.Command("get", "Get package")
	getUpdateFlag     = getCommand.Flag("update", "Update package to the latest").Short('u').Bool()
	getUseGitFlag     = getCommand.Flag("git", "Use git instead of zip archive file").Bool()
	getPackageName    = getCommand.Arg("package", "Package name on git repository").String()
	installCommand    = app.Command("install", "Install program")
	testCommand       = app.Command("test", "Test package")
	refreshTestFlag   = testCommand.Flag("refresh", "Refresh cache").Short('r').Bool()
	initCommand       = app.Command("init", "Initialize package")
	initAppCommand    = initCommand.Command("app", "Initialize application")
	appName           = initAppCommand.Arg("name", "Application name").String()
	appLicense        = initAppCommand.Flag("license", "License name").Short('l').Default("MIT").String()
	initLibCommand    = initCommand.Command("lib", "Initialize shared library")
	libName           = initLibCommand.Arg("name", "Library name").String()
	libLicense        = initLibCommand.Flag("license", "License name").Short('l').Default("MIT").String()
	addCommand        = app.Command("add", "Add source template")
	addClassCommand   = addCommand.Command("class", "Add class template")
	className         = addClassCommand.Arg("name", "Class name").Required().String()
	addTestCommand    = addCommand.Command("test", "Add test template")
	testName          = addTestCommand.Arg("test", "Test class name").Required().String()
	addLicenseCommand = addCommand.Command("license", "Add license file")
	licenseName       = addLicenseCommand.Arg("name", "License file name").String()
)

func main() {
	switch kingpin.MustParse(app.Parse(os.Args[1:])) {
	case buildCommand.FullCommand():
		Build(*refreshBuildFlag, *buildTypeFlag == "debug")
	case packCommand.FullCommand():
		Pack(*packTypeFlag == "debug")
	case cleanCommand.FullCommand():
		Clean()
	case getCommand.FullCommand():
		Get(*getPackageName, *getUpdateFlag, *getUseGitFlag)
	case installCommand.FullCommand():
		panic("not implemented yet")
	case testCommand.FullCommand():
		Test(*refreshTestFlag)
	case initAppCommand.FullCommand():
		InitApplication(*appName, *appLicense)
	case initLibCommand.FullCommand():
		InitLibrary(*libName, *libLicense)
	case addClassCommand.FullCommand():
		config, err := LoadConfig(".", true)
		if err != nil {
			log.Fatalln(err)
		}
		AddClass(config.Dir, *className, !config.IsApplication)
	case addTestCommand.FullCommand():
		config, err := LoadConfig(".", true)
		if err != nil {
			log.Fatalln(err)
		}
		AddTest(config.Dir, *testName)
	case addLicenseCommand.FullCommand():
		config, err := LoadConfig(".", true)
		if err != nil {
			log.Fatalln(err)
		}
		AddLicense(config, *licenseName)
	}
}
