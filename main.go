package main

import (
	"fmt"
	"github.com/fatih/color"
	"github.com/qtpm/qtpm/qtpmlib"
	"gopkg.in/alecthomas/kingpin.v2"
	"os"
	"runtime"
)

var (
	printTitle1 = color.New(color.FgWhite, color.Bold, color.BgGreen).SprintfFunc()
	printTitle2 = color.New(color.FgBlack, color.Bold, color.BgWhite).SprintfFunc()
	printBold   = color.New(color.Bold).SprintfFunc()
)

var qtmodules = []string{
	"3DCore",
	"3DInput",
	"3DLogic",
	"3DQuick",
	"3DQuickInput",
	"3DQuickRender",
	"3DRender",
	"AndroidExtras",
	"Bluetooth",
	"CLucene",
	"Concurrent",
	"Core",
	"DBus",
	"Designer",
	"DesignerComponents",
	"Gui",
	"Help",
	"LabsControls",
	"LabsTemplates",
	"Location",
	"MacExtras",
	"Multimedia",
	"MultimediaQuick_p",
	"MultimediaWidgets",
	"Network",
	"Nfc",
	"Positioning",
	"PrintSupport",
	"Qml",
	"Quick",
	"QuickParticles",
	"QuickTest",
	"QuickWidgets",
	"Sensors",
	"SerialBus",
	"SerialPort",
	"Sql",
	"Svg",
	"Test",
	"UiPlugin",
	"WebChannel",
	"WebEngine",
	"WebEngineCore",
	"WebEngineWidgets",
	"WebSockets",
	"WebView",
	"Widgets",
	"X11Extras",
	"Xml",
	"XmlPatterns",
	"WinExtras",
}

var (
	app     = kingpin.New("qtpm", "Package Manager fot Qt")
	asmJS   = app.Flag("asmjs", "Compile to asm.js").Bool()
	webAsm  = app.Flag("wasm", "Compile to WebAssembly").Bool()
	silent  = app.Flag("silent", "Set silent mode").Short('s').Bool()
	verbose = app.Flag("verbose", "Set verbose mode").Short('v').Bool()

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

	runCommand     = app.Command("run", "Build and launch program")
	runTypeFlag    = runCommand.Arg("build type", "release/debug").Default("debug").Enum("debug", "release")
	refreshRunFlag = runCommand.Flag("refresh", "Refresh cache").Short('r').Bool()

	packCommand  = app.Command("pack", "Create installer")
	packTypeFlag = packCommand.Arg("build type", "release/debug").Default("release").Enum("debug", "release")

	touchCommand  = app.Command("touch", "Recreate CMakeLists.txt")
	touchTypeFlag = touchCommand.Arg("build type", "release/debug").Default("debug").Enum("debug", "release")

	cleanCommand = app.Command("clean", "Clean temp files")

	docCommand = app.Command("doc", "Generate document")

	fmtCommand     = app.Command("fmt", "Format source code")
	fmtTargetFiles = fmtCommand.Arg("files", "Format target").ExistingFilesOrDirs()

	getCommand     = app.Command("get", "Get package")
	getUpdateFlag  = getCommand.Flag("update", "Update package to the latest").Short('u').Bool()
	getUseGitFlag  = getCommand.Flag("git", "Use git instead of zip archive file").Bool()
	getPackageName = getCommand.Arg("package", "Package name on git repository").String()

	testCommand = app.Command("test", "Test package")

	vetCommand     = app.Command("vet", "Check program")
	vetTargetFiles = vetCommand.Arg("files", "Check target").ExistingFilesOrDirs()

	envCommand  = app.Command("env", "Show command information")
	envVarsArgs = envCommand.Arg("var", "variable names").Strings()

	refreshTestFlag = testCommand.Flag("refresh", "Refresh cache").Short('r').Bool()

	addCommand             = app.Command("add", "Add source template")
	addClassCommand        = addCommand.Command("class", "Add class template with test")
	className              = addClassCommand.Arg("name", "Class name").Required().String()
	addTestCommand         = addCommand.Command("test", "Add test template")
	testName               = addTestCommand.Arg("test", "Test class name").Required().String()
	addQtModuleCommand     = addCommand.Command("qtmodule", "Add Qt module")
	qtModuleName           = addQtModuleCommand.Arg("name", "Qt module name").Required().Enum(qtmodules...)
	addLicenseCommand      = addCommand.Command("license", "Add license file")
	licenseName            = addLicenseCommand.Arg("name", "License file name").String()
	addDotGitIgnoreCommand = addCommand.Command(".gitignore", "Add .gitignore")

	linguistCommand  = app.Command("i18n", "i18n command")
	linguistAdd      = linguistCommand.Command("add", "Add language")
	linguistAddLang  = linguistAdd.Arg("lang", "Language (fr, ge, etc...").Required().String()
	linguistUpdate   = linguistCommand.Command("update", "Update translation source(.ts) file")
	linguistEdit     = linguistCommand.Command("edit", "Edit language")
	linguistEditLang = linguistEdit.Arg("lang", "Language (fr, ge, etc...").String()

	setupCommand        = app.Command("setup", "Setup additional tool")
	setupWebAsmCommand  = setupCommand.Command("webasm", "Setup WebAssembly tool")
	updateWebAsmCommand = setupWebAsmCommand.Flag("update", "Update").Bool()

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
	fmt.Println(printBold("\n%s - version %s by Yoshiki Shibukawa\n", logo, qtpm.QTPMVersionString))
}

func main() {
	app.HelpFlag.Short('h')
	command := kingpin.MustParse(app.Parse(os.Args[1:]))
	if *verbose {
		qtpm.SetVerbose()
	}
	switch command {
	case initAppCommand.FullCommand():
		printLogo()
		qtpm.InitApplication(*appName, *appLicense)
	case initLibCommand.FullCommand():
		printLogo()
		qtpm.InitLibrary(*libName, *libLicense)
	case buildCommand.FullCommand():
		printLogo()
		qtpm.SetUseWebAssemblyCompiler(*asmJS, *webAsm)
		qtpm.Build(*refreshBuildFlag, *buildTypeFlag == "debug")
	case runCommand.FullCommand():
		printLogo()
		qtpm.Run(*refreshRunFlag, *runTypeFlag == "debug")
	case packCommand.FullCommand():
		printLogo()
		qtpm.Pack(*packTypeFlag == "debug")
	case touchCommand.FullCommand():
		printLogo()
		qtpm.Touch(qtpm.MustLoadConfig(".", true), *touchTypeFlag == "debug", true)
	case cleanCommand.FullCommand():
		printLogo()
		qtpm.Clean()
	case docCommand.FullCommand():
		printLogo()
		qtpm.Doc()
	case fmtCommand.FullCommand():
		printLogo()
		qtpm.Format(*fmtTargetFiles)
	case getCommand.FullCommand():
		printLogo()
		qtpm.SetUseWebAssemblyCompiler(*asmJS, *webAsm)
		qtpm.Get(*getPackageName, *getUpdateFlag, *getUseGitFlag)
	case testCommand.FullCommand():
		printLogo()
		qtpm.Test(*refreshTestFlag)
	case vetCommand.FullCommand():
		printLogo()
		qtpm.Vet(*vetTargetFiles)
	case envCommand.FullCommand():
		qtpm.Env(*envVarsArgs)
	case addClassCommand.FullCommand():
		printLogo()
		config := qtpm.MustLoadConfig(".", true)
		qtpm.AddClass(config, *className)
		qtpm.AddTest(config, *className)
		qtpm.Touch(config, true, false)
	case addTestCommand.FullCommand():
		printLogo()
		config := qtpm.MustLoadConfig(".", true)
		qtpm.AddTest(config, *testName)
		qtpm.Touch(config, true, false)
	case addQtModuleCommand.FullCommand():
		printLogo()
		config := qtpm.MustLoadConfig(".", true)
		qtpm.AddQtModule(config, *qtModuleName)
		qtpm.Touch(config, true, false)
	case addLicenseCommand.FullCommand():
		printLogo()
		config := qtpm.MustLoadConfig(".", true)
		qtpm.AddLicense(config, *licenseName)
	case addDotGitIgnoreCommand.FullCommand():
		printLogo()
		config := qtpm.MustLoadConfig(".", true)
		qtpm.AddDotGitIgnore(config)
	case linguistAdd.FullCommand():
		printLogo()
		qtpm.LinguistAdd(*linguistAddLang, false)
	case linguistUpdate.FullCommand():
		printLogo()
		qtpm.LinguistUpdate()
	case linguistEdit.FullCommand():
		printLogo()
		qtpm.LinguistEdit(*linguistEditLang)
	case setupWebAsmCommand.FullCommand():
		printLogo()
		qtpm.SetupWebAssemblyTool(*updateWebAsmCommand)
	case versionCommand.FullCommand():
		fmt.Println(qtpm.QTPMVersionString)
	}
}
