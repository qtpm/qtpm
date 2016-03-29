package qtpm

import (
	"fmt"
	"github.com/fatih/color"
	"os"
	"path/filepath"
	"runtime"
	"strings"
)

func Pack(debugBuild bool) {
	config := MustLoadConfig(".", true)
	if !config.IsApplication {
		color.Red("pack command is for application package.\n")
		os.Exit(1)
	}
	os.MkdirAll(filepath.Join(config.Dir, "qtresources", "translations"), 0755)
	_, err := BuildPackage(config, config, false, debugBuild, true, false)
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
	} else if runtime.GOOS == "windows" {
		windeployqtPath := "windeployqt"
		if qtDir != "" {
			windeployqtPath = filepath.Join(qtDir, "bin", "windeployqt")
		}
		printSection("\nCreating Installer: %s\n", config.Name)
		var args []string
		if debugBuild {
			args = append(args, "--debug", strings.ToLower(config.Name)+".exe")
		} else {
			args = append(args, "--release", strings.ToLower(config.Name)+".exe")
		}
		cmd := Command(windeployqtPath, buildDirPath, args...)
		err := cmd.Run()
		if err != nil {
			color.Red("packaging error at windeployqt: %s\n", err.Error())
			os.Exit(1)
		}
		Touch(config, false, false)
		err = RunCMakeAndBuild(config.Dir, config, true, debugBuild, false, false)
		if err != nil {
			color.Red("packaging error at recreate CMakeLists.txt: %s\n", err.Error())
			os.Exit(1)
		}
		cmd2 := Command("cpack", buildDirPath)
		err = cmd2.Run()
		if err != nil {
			color.Red("packaging error at cpack: %s\n", err.Error())
			os.Exit(1)
		}
		printSuccess("\nFinish Build Successfully\n")
	} else {
		panic("it support only darwin")
	}
}
