package qtpm

import (
	"fmt"
	"log"
	"os"
	"path/filepath"
	"runtime"
)

func Pack(debugBuild bool) {
	config, err := LoadConfig(".", true)
	if err != nil {
		log.Fatalln(err)
	}
	if !config.IsApplication {
		log.Fatalln("pack command is for application package.")
	}
	os.MkdirAll(filepath.Join(config.Dir, "resources", "translations"), 0755)
	err = BuildPackage(config.Dir, config, false, debugBuild, !config.IsApplication)
	if err != nil {
		log.Fatalln(err)
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
		err := SequentialRun(buildDirPath).
			Run(macdeployqtPath, config.Name+".app", "-dmg").
			Run("mv", config.Name+".dmg", filepath.Join(config.Dir, "release", dmgFileName)).Finish()
		if err != nil {
			log.Fatalln("packaging error:", err.Error())
		}
		log.Println("Generated:", dmgFileName)
	} else {
		panic("it support only darwin")
	}
}
