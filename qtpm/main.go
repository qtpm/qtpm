package main

import (
	"fmt"
	"github.com/fatih/color"
	"github.com/qtpm/qtpm"
	"gopkg.in/alecthomas/kingpin.v2"
	"log"
	"os"
	"runtime"
)

var (
	printTitle1 = color.New(color.FgWhite, color.Bold, color.BgGreen).SprintfFunc()
	printTitle2 = color.New(color.FgBlack, color.Bold, color.BgWhite).SprintfFunc()
	printBold   = color.New(color.Bold).SprintfFunc()
)

var (
	app    = kingpin.New("qtpm", "Package Manager fot Qt")
	silent = app.Flag("silent", "Set silent mode").Short('v').Bool()

	initCommand    = app.Command("init", "Initialize package")
	initAppCommand = initCommand.Command("app", "Initialize application")
	appName        = initAppCommand.Arg("name", "Application name").String()
	appLicense     = initAppCommand.Flag("license", "License name").Short('l').Default("MIT").String()
	initLibCommand = initCommand.Command("lib", "Initialize shared library")
	libName        = initLibCommand.Arg("name", "Library name").String()
	libLicense     = initLibCommand.Flag("license", "License name").Short('l').Default("MIT").String()

	buildCommand     = app.Command("build", "Build program")
	buildTypeFlag    = buildCommand.Arg("build type", "release/debug").Default("debug").Enum("debug", "release")
	refreshBuildFlag = buildCommand.Flag("refresh", "Refresh cache").Short('r').Bool()

	packCommand  = app.Command("pack", "Create installer")
	packTypeFlag = packCommand.Arg("build type", "release/debug").Default("release").Enum("debug", "release")

	touchCommand  = app.Command("touch", "Recreate CMakeLists.txt")
	touchTypeFlag = touchCommand.Arg("build type", "release/debug").Default("debug").Enum("debug", "release")

	cleanCommand = app.Command("clean", "Clean temp files")

	docCommand = app.Command("doc", "Generate document")

	fmtCommand = app.Command("fmt", "Format source code")

	getCommand     = app.Command("get", "Get package")
	getUpdateFlag  = getCommand.Flag("update", "Update package to the latest").Short('u').Bool()
	getUseGitFlag  = getCommand.Flag("git", "Use git instead of zip archive file").Bool()
	getPackageName = getCommand.Arg("package", "Package name on git repository").String()

	testCommand = app.Command("test", "Test package")

	envCommand  = app.Command("env", "Show command information")
	envVarsArgs = envCommand.Arg("var", "variable names").Strings()

	refreshTestFlag = testCommand.Flag("refresh", "Refresh cache").Short('r').Bool()

	addCommand        = app.Command("add", "Add source template")
	addClassCommand   = addCommand.Command("class", "Add class template")
	className         = addClassCommand.Arg("name", "Class name").Required().String()
	addTestCommand    = addCommand.Command("test", "Add test template")
	testName          = addTestCommand.Arg("test", "Test class name").Required().String()
	addLicenseCommand = addCommand.Command("license", "Add license file")

	licenseName      = addLicenseCommand.Arg("name", "License file name").String()
	linguistCommand  = app.Command("i18n", "i18n command")
	linguistAdd      = linguistCommand.Command("add", "Add language")
	linguistAddLang  = linguistAdd.Arg("lang", "Language (fr, ge, etc...").Required().String()
	linguistUpdate   = linguistCommand.Command("update", "Update translation source(.ts) file")
	linguistEdit     = linguistCommand.Command("edit", "Edit language")
	linguistEditLang = linguistEdit.Arg("lang", "Language (fr, ge, etc...").String()

	versionCommand = app.Command("version", "Show version")
)

func printLogo() {
	if *silent {
		return
	}
	var logo string
	if runtime.GOOS == "windows" {
		logo = color.GreenString("Qtpm")
	} else {
		logo = printTitle1("Qt") + printTitle2("pm")
	}
	fmt.Println(printBold("\n%s - version %s by Yoshiki Shibukawa\n", logo, qtpm.Version))
}

func main() {
	app.HelpFlag.Short('h')
	switch kingpin.MustParse(app.Parse(os.Args[1:])) {
	case initAppCommand.FullCommand():
		printLogo()
		qtpm.InitApplication(*appName, *appLicense)
	case initLibCommand.FullCommand():
		printLogo()
		qtpm.InitLibrary(*libName, *libLicense)
	case buildCommand.FullCommand():
		printLogo()
		qtpm.Build(*refreshBuildFlag, *buildTypeFlag == "debug")
	case packCommand.FullCommand():
		printLogo()
		qtpm.Pack(*packTypeFlag == "debug")
	case touchCommand.FullCommand():
		printLogo()
		qtpm.Touch(*touchTypeFlag == "debug", true)
	case cleanCommand.FullCommand():
		printLogo()
		qtpm.Clean()
	case docCommand.FullCommand():
		printLogo()
		qtpm.Doc()
	case fmtCommand.FullCommand():
		printLogo()
		qtpm.Format()
	case getCommand.FullCommand():
		printLogo()
		qtpm.Get(*getPackageName, *getUpdateFlag, *getUseGitFlag)
	case testCommand.FullCommand():
		printLogo()
		qtpm.Test(*refreshTestFlag)
	case envCommand.FullCommand():
		qtpm.Env(*envVarsArgs)
	case addClassCommand.FullCommand():
		printLogo()
		config, err := qtpm.LoadConfig(".", true)
		if err != nil {
			log.Fatalln(err)
		}
		qtpm.AddClass(config, *className, !config.IsApplication)
		qtpm.AddTest(config, *className)
		qtpm.Touch(true, false)
	case addTestCommand.FullCommand():
		printLogo()
		config, err := qtpm.LoadConfig(".", true)
		if err != nil {
			log.Fatalln(err)
		}
		qtpm.AddTest(config, *testName)
		qtpm.Touch(true, false)
	case addLicenseCommand.FullCommand():
		printLogo()
		config, err := qtpm.LoadConfig(".", true)
		if err != nil {
			log.Fatalln(err)
		}
		qtpm.AddLicense(config, *licenseName)
	case linguistAdd.FullCommand():
		printLogo()
		qtpm.LinguistAdd(*linguistAddLang, false)
	case linguistUpdate.FullCommand():
		printLogo()
		qtpm.LinguistUpdate()
	case linguistEdit.FullCommand():
		printLogo()
		qtpm.LinguistEdit(*linguistEditLang)
	case versionCommand.FullCommand():
		fmt.Println(qtpm.Version)
	}
}
