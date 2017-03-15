package qtpm

import (
	"fmt"
	"os"
	"path/filepath"
	"runtime"
	"strings"

	"github.com/fatih/color"
)

func Pack(buildType BuildType, zipPack, wixPack bool) {
	config := MustLoadConfig(".", true)
	if !config.IsApplication {
		color.Red("pack command is for application package.\n")
		os.Exit(1)
	}
	os.MkdirAll(filepath.Join(config.Dir, "qtresources", "translations"), 0755)
	_, err := BuildPackage(config, config, buildType, false, true, false)
	if err != nil {
		color.Red("\nBuild Error\n")
		os.Exit(1)
	}
	os.MkdirAll(filepath.Join(config.Dir, "release"), 0755)
	buildDirPath := filepath.Join(config.Dir, BuildFolder(buildType))
	qtDir := FindQt(config.Dir)
	if runtime.GOOS == "darwin" {
		macdeployqtPath := "macdeployqt"
		if qtDir != "" {
			macdeployqtPath = filepath.Join(qtDir, "bin", "macdeployqt")
		}
		var dmgFileName string
		if buildType == Release {
			dmgFileName = fmt.Sprintf("%s-%d.%d.%d.dmg", config.Name, config.Version[0], config.Version[1], config.Version[2])
		} else {
			dmgFileName = fmt.Sprintf("%s-%d.%d.%d-%s.dmg", config.Name, config.Version[0], config.Version[1], config.Version[2], buildType.String())
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
		targetName := strings.ToLower(config.Name) + ".exe"
		// Visual Studio generates target file under Release/Debug folder
		if _, err := os.Stat(filepath.Join(buildDirPath, targetName)); err != nil {
			if buildType == Release {
				os.Rename(targetName, filepath.Join("Release", targetName))
			} else {
				os.Rename(targetName, filepath.Join("Debug", targetName))
			}
		}
		args := []string{
			"--release", targetName,
		}
		cmd := Command(windeployqtPath, buildDirPath, args...)
		err := cmd.Run()
		if err != nil {
			color.Red("packaging error at windeployqt: %s\n", err.Error())
			os.Exit(1)
		}
		Touch(config, buildType, false)
		err = RunCMakeAndBuild(config.Dir, config, buildType, true, false, false)
		if err != nil {
			color.Red("packaging error at recreate CMakeLists.txt: %s\n", err.Error())
			os.Exit(1)
		}
		var packArgs []string
		if zipPack {
			packArgs = append(packArgs, "-G", "ZIP")
		} else if wixPack {
			packArgs = append(packArgs, "-G", "WIX")
		}
		cmd2 := CommandWithPath("cpack", buildDirPath, FindWix(), packArgs...)
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
