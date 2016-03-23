package qtpm

import (
	"bytes"
	"fmt"
	"github.com/fatih/color"
	"log"
	"os"
	"path/filepath"
	"strings"
	"time"
)

type commandHelp struct {
	commands    []string
	description string
}

func dumpCommandHelp(helps []commandHelp) {
	var maxLength int
	for _, help := range helps {
		length := len(strings.Join(help.commands, " "))
		if length > maxLength {
			maxLength = length
		}
	}
	maxLength += 3
	for _, help := range helps {
		var buffer bytes.Buffer
		buffer.WriteString(color.BlueString("$ "))
		buffer.WriteString("qtpm ")
		length := 0
		for i, command := range help.commands {
			if i != 0 {
				buffer.WriteByte(' ')
				length++
			}
			if strings.HasPrefix(command, ":") {
				buffer.WriteString(color.MagentaString(command[1:]))
				length += (len(command) - 1)
			} else if strings.HasPrefix(command, "@") {
				buffer.WriteString(color.CyanString(command[1:]))
				length += (len(command) - 1)
			} else {
				buffer.WriteString(color.GreenString(command))
				length += len(command)
			}
		}
		for i := length; i < maxLength; i++ {
			buffer.WriteByte(' ')
		}
		buffer.WriteString(": ")
		buffer.WriteString(help.description)
		fmt.Println(buffer.String())
	}
}

var helpComment = `
Welcome to qtpm. qtpm provides several comannds to help your development:
`

var commonCommands = []commandHelp{
	commandHelp{
		commands:    []string{"add", "license", "@bsd"},
		description: "Change your project's license to BSD. All available names are in qtpm's README.",
	},
	commandHelp{
		commands:    []string{"add", "class", "@Message"},
		description: "Add src/message.cpp, src/message.h, test/message_test.cpp for Message class.",
	},
	commandHelp{
		commands:    []string{"add", "class", "@ErrorDialog@QDialog"},
		description: "Similar to the above command, but the class inherits QDialog class.",
	},
	commandHelp{
		commands:    []string{"test"},
		description: "Run tests",
	},
	commandHelp{
		commands:    []string{"fmt", ":[src/message.cpp...]"},
		description: "Clean your code.",
	},
	commandHelp{
		commands:    []string{"vet", ":[src/filesystem.cpp...]"},
		description: "Check your code.",
	},
	commandHelp{
		commands:    []string{"get"},
		description: "Download and build all required packages that is in qtpackage.toml.",
	},
	commandHelp{
		commands:    []string{"get", "@github.com/qtpm/QtCBOR"},
		description: "Download and specified package and add the name to qtpackage.toml.",
	},
}

var libCommands = []commandHelp{
	commandHelp{
		commands:    []string{"build", ":[release/debug]"},
		description: "Build library and deploy to dest folder.",
	},
}

var appCommands = []commandHelp{
	commandHelp{
		commands:    []string{"build", ":[release/debug]"},
		description: "Build application.",
	},
	commandHelp{
		commands:    []string{"pack"},
		description: "Build application and create installer.",
	},
	commandHelp{
		commands:    []string{"i18n", "add", "@ja"},
		description: "Add Japanese translation file(.ts).",
	},
	commandHelp{
		commands:    []string{"i18n", "update"},
		description: "Update all translation files.",
	},
	commandHelp{
		commands:    []string{"i18n", "edit", "@es"},
		description: "Edit Spanish translation file.",
	},
}

func showHelpForLib() {
	fmt.Println(helpComment)
	dumpCommandHelp(append(commonCommands, libCommands...))
}

func showHelpForApp() {
	fmt.Println(helpComment)
	dumpCommandHelp(append(commonCommands, appCommands...))
}

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
	variable := &ProjectDetail{
		config: config,
		Target: packageName,
	}
	AddClass(config, packageName, true)
	AddTest(config, packageName)
	WriteTemplate(".", "examples", "example.cpp", "main.cpp", variable, false)
	WriteTemplate(".", "", ".gitignore", "dotgitignore", variable, false)
	WriteTemplate(".", "", ".clang-format", "dotclang-format", variable, false)
	WriteTemplate(".", "", "CMakeExtra.txt", "CMakeExtra.txt", variable, false)
	WriteTemplate(".", "", "README.rst", "READMELib.rst", variable, false)
	Touch(true, false)
	showHelpForLib()
}

func InitApplication(name, license string) {
	_, packageName, _ := ParseName(name)
	config, dir := prepareProject(packageName, license)
	initDirs(dir, "Resource")
	config.Save()

	variable := &ProjectDetail{
		config:    config,
		Target:    packageName,
		QtModules: []string{"Widgets"},
	}
	WriteTemplate(".", "src", "main.cpp", "main.cpp", variable, false)
	WriteTemplate(".", "", ".gitignore", "dotgitignore", variable, false)
	WriteTemplate(".", "", ".clang-format", "dotclang-format", variable, false)
	WriteTemplate(".", "", "CMakeExtra.txt", "CMakeExtra.txt", variable, false)
	WriteTemplate(".", "", "README.rst", "READMEApp.rst", variable, false)
	Touch(true, false)
	showHelpForApp()
}
