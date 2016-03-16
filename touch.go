package qtpm

import (
	"github.com/fatih/color"
	"os"
	"path/filepath"
)

func Touch() {
	config, err := LoadConfig(".", true)
	if err != nil {
		color.Red(err.Error())
		os.Exit(1)
	}
	os.MkdirAll(filepath.Join(config.Dir, "resources", "translations"), 0755)
	err = BuildPackage(config.Dir, config, true, true, false, !config.IsApplication)
	if err != nil {
		color.Red("\nBuild Error\n")
		os.Exit(1)
	}
	printSuccess("\nFinish updating CMakeLists.txt successfully\n")
}
