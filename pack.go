package qtpm

import (
	"fmt"
	"github.com/fatih/color"
	"os"
	"path/filepath"
	"runtime"
)

func Pack(debugBuild bool) {
	config, err := LoadConfig(".", true)
	if err != nil {
		color.Red(err.Error())
		os.Exit(1)
	}
	if !config.IsApplication {
		color.Red("pack command is for application package.\n")
		os.Exit(1)
	}
	os.MkdirAll(filepath.Join(config.Dir, "resources", "translations"), 0755)
	err = BuildPackage(config.Dir, config, false, debugBuild, true, false)
	if err != nil {
		color.Red("\nBuild Error\n")
		os.Exit(1)
	}
	os.MkdirAll(filepath.Join(config.Dir, "release"), 0755)
	buildDirPath := filepath.Join(config.Dir, BuildFolder(debugBuild))
	qtDir := FindQt(config.Dir)
	if runtime.GOOS == "darwin" {
		macdeployqtPath := "macdeployqt"
		if qtDir != "" {
			macdeployqtPath = filepath.Join(qtDir, "bin", "macdeployqt")
		}
		var dmgFileName string
		if debugBuild {
			dmgFileName = fmt.Sprintf("%s-%d.%d.%d-debug.dmg", config.Name, config.Version[0], config.Version[1], config.Version[2])
		} else {
			dmgFileName = fmt.Sprintf("%s-%d.%d.%d.dmg", config.Name, config.Version[0], config.Version[1], config.Version[2])
		}
		printSection("\nCreating Installer: %s\n", config.Name)
		err := SequentialRun(buildDirPath).
			Run(macdeployqtPath, config.Name+".app", "-dmg").
			Run("mv", config.Name+".dmg", filepath.Join(config.Dir, "release", dmgFileName)).Finish()
		if err != nil {
			color.Red("packaging error: %s\n", err.Error())
			os.Exit(1)
		}
		printSuccess("\nGenerated: %s", dmgFileName)
		printSuccess("\nFinish Build Successfully\n")
	} else {
		panic("it support only darwin")
	}
}
