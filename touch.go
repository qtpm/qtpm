package qtpm

import (
	"github.com/fatih/color"
	"os"
)

func Touch(debug, verbose bool) {
	config, err := LoadConfig(".", true)
	if err != nil {
		color.Red(err.Error())
		os.Exit(1)
	}
	_, err = BuildPackage(config, config, true, debug, false, !config.IsApplication)
	if err != nil {
		color.Red("\nBuild Error\n")
		os.Exit(1)
	}
	if verbose {
		printSuccess("\nFinish updating CMakeLists.txt successfully\n")
	}
}
